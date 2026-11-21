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
#include "llvm/Support/MathExtras.h"

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

} // namespace

static const unsigned CPURegsDecoderTable[] = {
    CDM::R0, CDM::R1, CDM::R2, CDM::R3, CDM::R4, CDM::R5, CDM::R6, CDM::FP};

static DecodeStatus DecodeCPURegsRegisterClass(MCInst &MI, uint64_t RegNo,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  if (RegNo >= 8)
    return MCDisassembler::Fail;

  MI.addOperand(MCOperand::createReg(CPURegsDecoderTable[RegNo]));
  return MCDisassembler::Success;
}

// Decode the raw immediate and scale it to bytes if needed.
template <unsigned Scale>
static DecodeStatus decodeImm(MCInst &MI, uint64_t Imm, uint64_t Address,
                              const MCDisassembler *Decoder) {
  int64_t Value = static_cast<int64_t>(Imm) * Scale;
  MI.addOperand(MCOperand::createImm(Value));
  return MCDisassembler::Success;
}

// Interpret a 16-bit immediate as a signed value.
static DecodeStatus decodeImm16Signed(MCInst &MI, uint64_t Imm,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  int64_t Value = SignExtend64<16>(Imm);

  MI.addOperand(MCOperand::createImm(Value));

  return MCDisassembler::Success;
}

static DecodeStatus decodeShamt(MCInst &MI, uint64_t Imm, uint64_t Address,
                                const MCDisassembler *Decoder) {
  if (Imm >= 8)
    return MCDisassembler::Fail;

  MI.addOperand(MCOperand::createImm(Imm + 1));
  return MCDisassembler::Success;
}

static DecodeStatus decodeBranchAbsolute(MCInst &MI, uint32_t Insn,
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

// Decode a negative imm6/imm9 encoding into the regular instruction.
template <unsigned ImmBits, unsigned Scale, unsigned RegOperands>
static DecodeStatus decodeNegativeImm(MCInst &MI, uint16_t Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {

  static_assert(ImmBits == 6 || ImmBits == 9,
                "Expected an imm6 or imm9 encoding");
  static_assert(Scale == 1 || Scale == 2, "Expected byte or word scaling");
  static_assert((ImmBits == 6 && (RegOperands == 1 || RegOperands == 2)) ||
                    (ImmBits == 9 && RegOperands == 0),
                "Invalid register count for this encoding");

  constexpr unsigned ImmShift = ImmBits == 6 ? 3 : 0;
  constexpr unsigned ImmMask = (1u << ImmBits) - 1;

  unsigned RawImm = (Insn >> ImmShift) & ImmMask;
  int64_t Value =
      (static_cast<int64_t>(RawImm) - (int64_t{1} << ImmBits)) * Scale;

  for (unsigned I = 0; I < RegOperands; ++I) {
    unsigned Reg = Insn & 0x7;
    if (DecodeCPURegsRegisterClass(MI, Reg, Address, Decoder) ==
        MCDisassembler::Fail)
      return MCDisassembler::Fail;
  }

  MI.addOperand(MCOperand::createImm(Value));
  return MCDisassembler::Success;
}

static DecodeStatus decodeBranchRelative(MCInst &MI, uint16_t Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  unsigned Prefix = Insn >> 13;
  unsigned Cond = (Insn >> 9) & 0xF;
  unsigned RawImm = Insn & 0x1FF;

  MI.clear();

  if (Cond == CDMCOND::FALSE) {
    MI.setOpcode(CDM::NOP);
    return MCDisassembler::Success;
  }

  int64_t WordOffset =
      static_cast<int64_t>(RawImm) - (Prefix == 0b110 ? 512 : 0);
  int64_t Target = static_cast<int64_t>(Address & 0xFFFF) + 2 + WordOffset * 2;

  if (Cond == CDMCOND::TRUE) {
    MI.setOpcode(CDM::BRImm9);
  } else {
    MI.setOpcode(CDM::BCondImm9);
    MI.addOperand(MCOperand::createImm(Cond));
  }

  MI.addOperand(MCOperand::createImm(Target & 0xFFFF));
  return MCDisassembler::Success;
}

#include "CDMGenDisassemblerTables.inc"

static bool is32BitInstruction(uint16_t FirstWord) {
  if ((FirstWord & 0xFFF8u) == 0x2010u)
    return true;

  if (FirstWord == 0x0008u)
    return true;

  if ((FirstWord & 0xFFF0u) == 0x0800u)
    return true;

  return false;
}

DecodeStatus CDMDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                             ArrayRef<uint8_t> Bytes,
                                             uint64_t Address,
                                             raw_ostream &CStream) const {

  Size = 0;

  if (Bytes.size() < 2)
    return MCDisassembler::Fail;

  uint16_t FirstWord = support::endian::read16le(Bytes.data());

  if (is32BitInstruction(FirstWord)) {
    if (Bytes.size() < 4) {
      return MCDisassembler::Fail;
    }

    uint32_t Inst = support::endian::read32le(Bytes.data());

    DecodeStatus Result = decodeInstruction(DecoderTable32, MI, Inst, Address,
                                            this, getSubtargetInfo());

    Size = 4;
    return Result;
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