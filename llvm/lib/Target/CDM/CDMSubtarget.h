
//
// Created by ilya on 21.10.23.
//

#ifndef LLVM_CDMSUBTARGET_H
#define LLVM_CDMSUBTARGET_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#include "CDMFrameLowering.h"
#include "CDMISelLowering.h"
#include "CDMInstrInfo.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"

#define GET_SUBTARGETINFO_HEADER
#include "CDMGenSubtargetInfo.inc"

namespace llvm {

class CDMSubtarget : public CDMGenSubtargetInfo {
public:
  CDMSubtarget(StringRef CPU, StringRef FS, const CDMTargetMachine &TM);

  const CDMTargetLowering *getTargetLowering() const override { return &TLInfo; }
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

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

#define GET_SUBTARGETINFO_MACRO(ATTRIBUTE, DEFAULT, GETTER)                    \
  bool GETTER() const { return ATTRIBUTE; }
#include "CDMGenSubtargetInfo.inc"

private:
#define GET_SUBTARGETINFO_MACRO(ATTRIBUTE, DEFAULT, GETTER)                    \
  bool ATTRIBUTE = DEFAULT;
#include "CDMGenSubtargetInfo.inc"

  CDMFrameLowering FrameLowering;
  CDMInstrInfo InstrInfo;
  CDMTargetLowering TLInfo;
  SelectionDAGTargetInfo TSInfo;

  CDMSubtarget &initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                                const TargetMachine &TM);
};

} // namespace llvm

#endif // LLVM_CDMSUBTARGET_H
