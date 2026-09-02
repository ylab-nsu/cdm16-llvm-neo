
#include "TargetInfo/CDMTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheCDMTarget() {
  static Target TheCDMTarget;
  return TheCDMTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCDMTargetInfo() {
  RegisterTarget<Triple::cdm, /*HasJIT=*/false> X(
      getTheCDMTarget(), "cdm", "CdM-16 educational processor", "CDM");
}
