#include "CDMTargetObjectFile.h"

namespace llvm {
void CDMTargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM) {
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
}
} // namespace llvm
