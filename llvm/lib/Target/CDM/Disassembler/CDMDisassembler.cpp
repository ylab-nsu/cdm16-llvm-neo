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
    CDM::R0, CDM::R1, CDM::R2, CDM::R3,
    CDM::R4, CDM::R5, CDM::R6, CDM::FP};

static DecodeStatus
DecodeCPURegsRegisterClass(MCInst &MI, uint64_t RegNo, uint64_t Address,
                           const MCDisassembler *Decoder) {
  if (RegNo >= 8)
    return MCDisassembler::Fail;

  MI.addOperand(MCOperand::createReg(CPURegsDecoderTable[RegNo]));
  return MCDisassembler::Success;
}

// Decode the raw 6-bit byte offset.
// getInstruction restores the sign from the original opcode.
static DecodeStatus
DecodeStackByteOffset(MCInst &MI, uint64_t Imm, uint64_t Address,
                      const MCDisassembler *Decoder) {
  int64_t Offset = static_cast<int64_t>(Imm);
  MI.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

// Decode the raw 9-bit stack offset and convert words to bytes.
// getInstruction restores the sign from the original opcode.
static DecodeStatus
DecodeStackWordOffset(MCInst &MI, uint64_t Imm, uint64_t Address,
                      const MCDisassembler *Decoder) {
  int64_t Offset = static_cast<int64_t>(Imm) * 2;
  MI.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

// Decode the raw 6-bit FP-relative offset and convert words to bytes.
// getInstruction restores the sign from the original opcode.
static DecodeStatus
DecodeFrameWordOffset(MCInst &MI, uint64_t Imm, uint64_t Address,
                      const MCDisassembler *Decoder) {
  int64_t Offset = static_cast<int64_t>(Imm) * 2;
  MI.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

// Decode the raw 6-bit immediate used by ALU instructions and LDI.
// getInstruction restores the sign from the original opcode.
static DecodeStatus
DecodeImm6(MCInst &MI, uint64_t Imm, uint64_t Address,
                 const MCDisassembler *Decoder) {
  MI.addOperand(MCOperand::createImm(static_cast<int64_t>(Imm)));
  return MCDisassembler::Success;
}

// Decode the raw 9-bit immediate used by PUSH and JSR.
// getInstruction restores the sign from the original opcode.
static DecodeStatus
DecodeImm9(MCInst &MI, uint64_t Imm, uint64_t Address,
                  const MCDisassembler *Decoder) {
  MI.addOperand(MCOperand::createImm(static_cast<int64_t>(Imm)));
  return MCDisassembler::Success;
}

// Interpret a 16-bit immediate as a signed value.
static DecodeStatus
DecodeSignedImm16(MCInst &MI, uint64_t Imm, uint64_t Address,
                  const MCDisassembler *Decoder) {
  int64_t Value = SignExtend64<16>(Imm);

  MI.addOperand(MCOperand::createImm(Value));

  return MCDisassembler::Success;
}

static DecodeStatus
DecodeShamt(MCInst &MI, uint64_t Imm, uint64_t Address,
            const MCDisassembler *Decoder) {
  if (Imm >= 8)
    return MCDisassembler::Fail;

  MI.addOperand(MCOperand::createImm(Imm + 1));
  return MCDisassembler::Success;
}

#include "CDMGenDisassemblerTables.inc"

static bool is32BitInstruction(uint16_t FirstWord) {
  // LDI rd, imm16
  if ((FirstWord & 0xFFF8u) == 0x2010u)
    return true;

  // JSR imm16
  if (FirstWord == 0x0008u)
    return true;

  // BR / BCond imm16
  if ((FirstWord & 0xFFF0u) == 0x0800u)
    return true;

  return false;
}

DecodeStatus CDMDisassembler::getInstruction(
    MCInst &MI, uint64_t &Size, ArrayRef<uint8_t> Bytes,
    uint64_t Address, raw_ostream &CStream) const {

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

  // Decode both positive and negative relative branches.
  if ((FirstWord >> 13) == 0b110 ||
      (FirstWord >> 13) == 0b111) {

    // Preserve the dedicated NOP instruction.
    if (FirstWord != 0xFFFFu) {
      unsigned Prefix = FirstWord >> 13;
      unsigned Cond = (FirstWord >> 9) & 0xF;
      unsigned Imm = FirstWord & 0x1FF;

      int64_t WordOffset =
          static_cast<int64_t>(Imm) -
          (Prefix == 0b110 ? 512 : 0);

      // PC points to the next instruction after the branch.
      int64_t Target =
          static_cast<int64_t>(Address) + 2 + WordOffset * 2;

      MI.clear();

      if (Cond == 0xE) {
        MI.setOpcode(CDM::BRImm9);
      } else {
        MI.setOpcode(CDM::BCondImm9);
        MI.addOperand(MCOperand::createImm(Cond));
      }

      MI.addOperand(MCOperand::createImm(Target & 0xFFFF));

      Size = 2;
      return MCDisassembler::Success;
    }
  }

  Size = 2;

  bool IsNegativeADDImm6 = (FirstWord & 0xFE00u) == 0x7A00u;
  bool IsNegativeLDIImm6 = (FirstWord & 0xFE00u) == 0x7600u;
  bool IsNegativeLSW = (FirstWord & 0xFE00u) == 0x6200u;
  bool IsNegativeLSB = (FirstWord & 0xFE00u) == 0x6600u;
  bool IsNegativeLSSB = (FirstWord & 0xFE00u) == 0x6A00u;
  bool IsNegativeSSW = (FirstWord & 0xFE00u) == 0x6E00u;
  bool IsNegativeSSB = (FirstWord & 0xFE00u) == 0x7200u;
  bool IsNegativeCMPImm6 = (FirstWord & 0xFE00u) == 0x7E00u;
  bool IsNegativePUSHImm9 = (FirstWord & 0xFE00u) == 0x8600u;
  bool IsNegativeADDSPImm9 = (FirstWord & 0xFE00u) == 0x8A00u;
  bool IsNegativeJSRImm9 = (FirstWord & 0xFE00u) == 0x8E00u;

  uint16_t DecodeWord = FirstWord;
  if (IsNegativeADDImm6 || IsNegativeLDIImm6 || IsNegativeLSW ||
      IsNegativeLSB || IsNegativeLSSB || IsNegativeSSW || IsNegativeSSB ||
      IsNegativeCMPImm6 || IsNegativePUSHImm9 || IsNegativeADDSPImm9 ||
      IsNegativeJSRImm9)
    DecodeWord &= ~(1u << 9);

  DecodeStatus Result = decodeInstruction(DecoderTable16, MI, DecodeWord,
                                        Address, this, getSubtargetInfo());

  if (Result != MCDisassembler::Fail) {
    if (IsNegativeADDImm6 && MI.getOpcode() == CDM::ADDImm6) {
      MCOperand &Imm = MI.getOperand(2);
      Imm.setImm(Imm.getImm() - 64);
    } else if (IsNegativeLDIImm6 && MI.getOpcode() == CDM::LDIImm6) {
      MCOperand &Imm = MI.getOperand(1);
      Imm.setImm(Imm.getImm() - 64);
    } else if (IsNegativeLSW && MI.getOpcode() == CDM::LSW) {
      // The operand decoder has already converted words to bytes.
      MCOperand &Imm = MI.getOperand(1);
      Imm.setImm(Imm.getImm() - 128);
    } else if (IsNegativeLSB && MI.getOpcode() == CDM::LSB) {
      MCOperand &Imm = MI.getOperand(1);
      Imm.setImm(Imm.getImm() - 64);
    } else if (IsNegativeLSSB && MI.getOpcode() == CDM::LSSB) {
      MCOperand &Imm = MI.getOperand(1);
      Imm.setImm(Imm.getImm() - 64);
    } else if (IsNegativeSSW && MI.getOpcode() == CDM::SSW) {
      // The operand decoder has already converted words to bytes.
      MCOperand &Imm = MI.getOperand(1);
      Imm.setImm(Imm.getImm() - 128);
    } else if (IsNegativeSSB && MI.getOpcode() == CDM::SSB) {
      MCOperand &Imm = MI.getOperand(1);
      Imm.setImm(Imm.getImm() - 64);
    } else if (IsNegativeCMPImm6 && MI.getOpcode() == CDM::CMPImm6) {
      MCOperand &Imm = MI.getOperand(1);
      Imm.setImm(Imm.getImm() - 64);
    } else if (IsNegativePUSHImm9 && MI.getOpcode() == CDM::PUSHImm9) {
      MCOperand &Imm = MI.getOperand(0);
      Imm.setImm(Imm.getImm() - 512);
    } else if (IsNegativeADDSPImm9 && MI.getOpcode() == CDM::ADDSPImm9) {
      // The operand decoder has already converted words to bytes.
      MCOperand &Imm = MI.getOperand(0);
      Imm.setImm(Imm.getImm() - 1024);
    } else if (IsNegativeJSRImm9 && MI.getOpcode() == CDM::JSRImm9) {
      MCOperand &Imm = MI.getOperand(0);
      Imm.setImm(Imm.getImm() - 512);
    }
  }
  return Result;
}

static MCDisassembler *
createCDMDisassembler(const Target &T, const MCSubtargetInfo &STI,
                      MCContext &Ctx) {
  return new CDMDisassembler(STI, Ctx);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeCDMDisassembler() {
  TargetRegistry::RegisterMCDisassembler(getTheCDMTarget(),
                                         createCDMDisassembler);
}