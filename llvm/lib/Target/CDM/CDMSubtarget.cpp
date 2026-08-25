//
// Created by ilya on 21.10.23.
//

#include "CDMSubtarget.h"
#include "CDMTargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "cdm-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "CDMGenSubtargetInfo.inc"

CDMSubtarget::CDMSubtarget(StringRef CPU, StringRef FS,
                           const CDMTargetMachine &TM)
    : CDMGenSubtargetInfo(TM.getTargetTriple(), CPU, CPU, FS),
      FrameLowering(initializeSubtargetDependencies(CPU, FS, TM)), InstrInfo(),
      TLInfo(TM, *this)

{
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);
}

CDMSubtarget &
CDMSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                              const TargetMachine &TM) {
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);
  return *this;
}
