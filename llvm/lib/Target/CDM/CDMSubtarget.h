//
// Created by ilya on 21.10.23.
//

#ifndef LLVM_CDMSUBTARGET_H
#define LLVM_CDMSUBTARGET_H

#include "CDMISelLowering.h"

#include "CDMFrameLowering.h"
#include "CDMInstrInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/TargetParser/Triple.h"

#define GET_SUBTARGETINFO_HEADER
#include "CDMGenSubtargetInfo.inc"

namespace llvm {

class CDMSubtarget : public CDMGenSubtargetInfo {
  SelectionDAGTargetInfo TSInfo;

public:
  CDMSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
               const CDMTargetMachine &TM);

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const CDMISelLowering *getTargetLowering() const override { return &TLInfo; }
  const TargetFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const CDMInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const CDMRegisterInfo *getRegisterInfo() const override {
    return &InstrInfo.getRegisterInfo();
  }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }

#define GET_SUBTARGETINFO_MACRO(ATTRIBUTE, DEFAULT, GETTER)                    \
  bool GETTER() const { return ATTRIBUTE; }
#include "CDMGenSubtargetInfo.inc"

private:
  CDMFrameLowering FrameLowering;
  CDMInstrInfo InstrInfo;
  CDMISelLowering TLInfo;

#define GET_SUBTARGETINFO_MACRO(ATTRIBUTE, DEFAULT, GETTER)                    \
  bool ATTRIBUTE = DEFAULT;
#include "CDMGenSubtargetInfo.inc"

  CDMSubtarget &initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                                const TargetMachine &TM);
};

} // namespace llvm

#endif // LLVM_CDMSUBTARGET_H
