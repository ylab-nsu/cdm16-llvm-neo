//
// Created by ilya on 21.10.23.
//

#include "CDMSubtarget.h"

#include "llvm/MC/TargetRegistry.h"

#include "CDM.h"
#include "CDMTargetMachine.h"

#define DEBUG_TYPE "cdm-subtarget"

using namespace llvm;

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "CDMGenSubtargetInfo.inc"

CDMSubtarget::CDMSubtarget(StringRef CPU, StringRef FS,
                           const CDMTargetMachine &TM)
    : CDMGenSubtargetInfo(TM.getTargetTriple(), CPU, CPU, FS),
      FrameLowering(initializeSubtargetDependencies(CPU, FS, TM)), InstrInfo(),
      TLInfo(TM, *this) {}

CDMSubtarget &
CDMSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                              const TargetMachine &TM) {
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);
  return *this;
}
