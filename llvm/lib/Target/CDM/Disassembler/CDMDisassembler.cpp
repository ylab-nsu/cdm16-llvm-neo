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