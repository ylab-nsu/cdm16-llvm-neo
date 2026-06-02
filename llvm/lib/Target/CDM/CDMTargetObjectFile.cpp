#include "CDMTargetObjectFile.h"
#include "CDM.h"

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"

namespace llvm {
void CDMTargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM) {
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
  ProgmemDataSection =
      Ctx.getELFSection(".progmem.data", ELF::SHT_PROGBITS, ELF::SHF_ALLOC);
}

MCSection *CDMTargetObjectFile::SelectSectionForGlobal(
    const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const {

  if (isProgramMemoryAddress(GO) && !GO->hasSection() && Kind.isReadOnly()) {
    return ProgmemDataSection;
  }

  return TargetLoweringObjectFileELF::SelectSectionForGlobal(GO, Kind, TM);
}

} // namespace llvm
