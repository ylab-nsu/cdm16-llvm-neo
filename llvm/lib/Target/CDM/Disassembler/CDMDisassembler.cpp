#include "CDMInstrInfo.h"

#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "TargetInfo/CDMTargetInfo.h"

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Endian.h"

using namespace llvm;

#define DEBUG_TYPE "cdm-disassembler"

using DecodeStatus = MCDisassembler::DecodeStatus;

namespace {

class CDMDisassembler : public MCDisassembler {
public:
  CDMDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  DecodeStatus getInstruction(MCInst &MI, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};

enum class Sign { Positive, Negative };

} // namespace

static DecodeStatus DecodeCPURegsRegisterClass(MCInst &MI, uint64_t RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  static constexpr unsigned DecoderTable[] = {
      CDM::R0, CDM::R1, CDM::R2, CDM::R3, CDM::R4, CDM::R5, CDM::R6, CDM::FP,
  };

  if (RegNo >= 8) {
    return MCDisassembler::Fail;
  }

  MI.addOperand(MCOperand::createReg(DecoderTable[RegNo]));
  return MCDisassembler::Success;
}

template <unsigned Align>
static DecodeStatus decodeOffset(MCInst &MI, uint64_t Imm, uint64_t Address,
                                 const MCDisassembler *Decoder) {
  int64_t Value = static_cast<int64_t>(Imm) * Align;
  MI.addOperand(MCOperand::createImm(Value));
  return MCDisassembler::Success;
}

static DecodeStatus decodeShamt(MCInst &MI, uint64_t Imm, uint64_t Address,
                                const MCDisassembler *Decoder) {
  if (Imm >= 8) {
    return MCDisassembler::Fail;
  }

  MI.addOperand(MCOperand::createImm(Imm + 1));
  return MCDisassembler::Success;
}

// Decodes an instruction with a negative imm6 or imm9 operand.
//
// ImmBits is the size of the immediate operand. Must be 6 or 9.
// Align is the alignment of the operand if it is an address.
// NumRegOperands is the number of register operands preceding
//   the immediate operand (including dummy outputs tied to inputs)
template <unsigned ImmBits, unsigned Align, unsigned NumRegOperands>
static DecodeStatus decodeInstImmNegative(MCInst &MI, uint16_t Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  static_assert(ImmBits == 6 || ImmBits == 9,
                "Expected an imm6 or imm9 encoding");

  constexpr unsigned ImmShift = ImmBits == 6 ? 3 : 0;
  constexpr unsigned ImmMask = (1u << ImmBits) - 1;

  unsigned RawImm = (Insn >> ImmShift) & ImmMask;
  int64_t Value =
      (static_cast<int64_t>(RawImm) - (int64_t{1} << ImmBits)) * Align;

  for (unsigned I = 0; I < NumRegOperands; ++I) {
    unsigned Reg = Insn & 0x7;
    // We expect either 1 register or no registers in this function.
    // Some instructions, like ADDImm6 have 2 register operands in LLVM,
    // but they are tied together (in/out), so we assign the same value to both.
    if (DecodeCPURegsRegisterClass(MI, Reg, Address, Decoder) ==
        MCDisassembler::Fail) {
      return MCDisassembler::Fail;
    }
  }

  MI.addOperand(MCOperand::createImm(Value));
  return MCDisassembler::Success;
}

static DecodeStatus decodeInstBranchAbsolute(MCInst &MI, uint32_t Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  unsigned Cond = Insn & 0xF;
  unsigned Target = (Insn >> 16) & 0xFFFF;

  MI.clear();

  if (Cond == CDMCOND::FALSE) {
    MI.setOpcode(CDM::NOP);
  } else {
    MI.setOpcode(CDM::BCondImm16);
    MI.addOperand(MCOperand::createImm(Cond));
    MI.addOperand(MCOperand::createImm(Target));
  }

  return MCDisassembler::Success;
}

template <Sign OperandSign>
static DecodeStatus decodeInstBranchRelative(MCInst &MI, uint16_t Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  unsigned Cond = (Insn >> 9) & 0xF;
  unsigned RawImm = Insn & 0x1FF;

  MI.clear();

  if (Cond == CDMCOND::FALSE) {
    MI.setOpcode(CDM::NOP);
    return MCDisassembler::Success;
  }

  // Here we pre-add the address of the instruction to the operand,
  // because relative branches store the absolute target address in their
  // immediate operand.
  //
  // We do this because there is no way to represent a branch with a PC offset
  // in text.
  //
  // The disassembly will not assemble to the same image byte-per-byte, but it
  // at least would have the same semantics.
  int64_t WordOffset =
      static_cast<int64_t>(RawImm) + (OperandSign == Sign::Positive ? 0 : -512);
  int64_t Target = static_cast<int64_t>(Address & 0xFFFF) + 2 + WordOffset * 2;

  if (Cond == CDMCOND::TRUE) {
    // We have a separate instruction for unconditional branches.
    // Maybe refactor out?
    MI.setOpcode(CDM::BRImm9);
  } else {
    MI.setOpcode(CDM::BCondImm9);
    MI.addOperand(MCOperand::createImm(Cond));
  }
  MI.addOperand(MCOperand::createImm(Target & 0xFFFF));
  return MCDisassembler::Success;
}

template <Sign OperandSign>
static DecodeStatus decodeInstJSRRelative(MCInst &MI, uint16_t Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  unsigned RawImm = Insn & 0x1FF;

  MI.clear();

  // Here we pre-add the address of the instruction to the operand,
  // same as in PC-relative branches.
  int64_t WordOffset =
      static_cast<int64_t>(RawImm) + (OperandSign == Sign::Positive ? 0 : -512);
  int64_t Target = static_cast<int64_t>(Address & 0xFFFF) + 2 + WordOffset * 2;

  MI.setOpcode(CDM::JSRImm9);
  MI.addOperand(MCOperand::createImm(Target & 0xFFFF));
  return MCDisassembler::Success;
}

#include "CDMGenDisassemblerTables.inc"

static bool is32BitInstruction(uint16_t FirstWord) {
  // ldi imm16
  if ((FirstWord & 0xE078u) == 0x2010u) {
    return true;
  }
  // jsr imm16
  if ((FirstWord & 0xF80Fu) == 0x0008u) {
    return true;
  }
  // br imm16
  if ((FirstWord & 0xFFF0u) == 0x0800u) {
    return true;
  }
  return false;
}

DecodeStatus CDMDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                             ArrayRef<uint8_t> Bytes,
                                             uint64_t Address,
                                             raw_ostream &CStream) const {
  Size = 0;
  if (Bytes.size() < 2) {
    return MCDisassembler::Fail;
  }

  uint16_t FirstWord = support::endian::read16le(Bytes.data());

  if (is32BitInstruction(FirstWord)) {
    if (Bytes.size() < 4) {
      return MCDisassembler::Fail;
    }

    uint32_t Inst = support::endian::read32le(Bytes.data());

    Size = 4;
    return decodeInstruction(DecoderTable32, MI, Inst, Address, this,
                             getSubtargetInfo());
  }

  Size = 2;
  return decodeInstruction(DecoderTable16, MI, FirstWord, Address, this,
                           getSubtargetInfo());
}

static MCDisassembler *createCDMDisassembler(const Target &T,
                                             const MCSubtargetInfo &STI,
                                             MCContext &Ctx) {
  return new CDMDisassembler(STI, Ctx);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeCDMDisassembler() {
  TargetRegistry::RegisterMCDisassembler(getTheCDMTarget(),
                                         createCDMDisassembler);
}
