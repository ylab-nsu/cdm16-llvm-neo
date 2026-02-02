#ifndef LLVM_CDMASMBACKEND_H
#define LLVM_CDMASMBACKEND_H

#include "MCTargetDesc/CDMFixupKinds.h"

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/TargetParser/Triple.h"

namespace llvm {

class MCAssembler;
class MCContext;
struct MCFixupKindInfo;

class CDMAsmBackend : public MCAsmBackend {
public:
  CDMAsmBackend(Triple::OSType OSType)
      : MCAsmBackend(llvm::endianness::little), OSType(OSType) {}

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  void applyFixup(const MCFragment &, const MCFixup &, const MCValue &Target,
                  MutableArrayRef<char> Data, uint64_t Value,
                  bool IsResolved) override;

  std::optional<MCFixupKind> getFixupKind(StringRef Name) const override;
  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override;

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;

  bool mayNeedRelaxation(unsigned Opcode, ArrayRef<MCOperand> Operands,
                         const MCSubtargetInfo &STI) const override;
  bool fixupNeedsRelaxation(const MCFixup &Fixup,
                            uint64_t Value) const override;
  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;

  void adjustFixupValue(const MCFixup &Fixup, const MCValue &Target,
                        uint64_t &Value, MCContext *Ctx) const;

private:
  Triple::OSType OSType;
};

} // end namespace llvm

#endif // LLVM_CDMASMBACKEND_H
