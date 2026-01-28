#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/EndianStream.h"

using namespace llvm;

namespace {
class CDMMCCodeEmitter : public MCCodeEmitter {
  CDMMCCodeEmitter(const CDMMCCodeEmitter &) = delete;
  void operator=(const CDMMCCodeEmitter &) = delete;
  MCContext &Ctx;
  MCInstrInfo const &MCII;

public:
  CDMMCCodeEmitter(MCInstrInfo const &MCII, MCContext &Ctx)
      : Ctx(Ctx), MCII(MCII) {}

  ~CDMMCCodeEmitter() override = default;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  /// TableGen'erated function for getting the binary encoding for an
  /// instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
};


} // end anonymous namespace

void CDMMCCodeEmitter::encodeInstruction(const MCInst &MI,
                                         SmallVectorImpl<char> &CB,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  unsigned Size = Desc.getSize();

  switch (Size) {
  default:
    llvm_unreachable("Unhandled encodeInstruction length!");
  case 2: {
    uint16_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
    support::endian::write<uint16_t>(CB, Bits, llvm::endianness::little);
    break;
  }
  case 4: {
    uint32_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
    support::endian::write<uint32_t>(CB, Bits, llvm::endianness::little);
    break;
  }
  }
}

MCCodeEmitter *llvm::createCDMMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx) {
  return new CDMMCCodeEmitter(MCII, Ctx);
}

#include "CDMGenMCCodeEmitter.inc"
