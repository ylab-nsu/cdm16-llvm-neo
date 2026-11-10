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

// Decode a 7-bit signed stack offset stored directly in bytes.
// The generated decoder extracts the raw bits without sign extension.
static DecodeStatus
DecodeStackByteOffset(MCInst &MI, uint64_t Imm, uint64_t Address,
                      const MCDisassembler *Decoder) {
  int64_t Offset = SignExtend64<7>(Imm);
  MI.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

// Decode a 10-bit signed stack offset.
// The encoded value is measured in 2-byte words, so the
// sign-extended value must be multiplied by 2.
static DecodeStatus
DecodeStackWordOffset(MCInst &MI, uint64_t Imm, uint64_t Address,
                      const MCDisassembler *Decoder) {
  int64_t Offset = SignExtend64<10>(Imm) * 2;
  MI.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

// Decode a signed 7-bit FP-relative word offset.
// The encoded offset is measured in 2-byte words.
static DecodeStatus
DecodeFrameWordOffset(MCInst &MI, uint64_t Imm, uint64_t Address,
                      const MCDisassembler *Decoder) {
  int64_t Offset = SignExtend64<7>(Imm) * 2;
  MI.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

// Decode a signed 7-bit immediate used by ALU instructions.
static DecodeStatus
DecodeSignedImm7(MCInst &MI, uint64_t Imm, uint64_t Address,
                 const MCDisassembler *Decoder) {
  MI.addOperand(MCOperand::createImm(SignExtend64<7>(Imm)));
  return MCDisassembler::Success;
}

// Decode a signed 10-bit immediate used by PUSH.
static DecodeStatus
DecodeSignedImm10(MCInst &MI, uint64_t Imm, uint64_t Address,
                  const MCDisassembler *Decoder) {
  MI.addOperand(MCOperand::createImm(SignExtend64<10>(Imm)));
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

  return decodeInstruction(DecoderTable16, MI, FirstWord, Address,
                           this, getSubtargetInfo());
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