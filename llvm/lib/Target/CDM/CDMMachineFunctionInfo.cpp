//
// Created by ilya on 20.11.23.
//

#include "CDMMachineFunctionInfo.h"

#include "llvm/IR/CallingConv.h"

using namespace llvm;

static bool hasOneUsedArgument(const Function &F) {
  return F.arg_size() == 1 && !F.arg_begin()->use_empty();
}

CDMMachineFunctionInfo::CDMMachineFunctionInfo(const Function &F,
                                               const TargetSubtargetInfo *STI)
    : IsInterruptHandler(F.getCallingConv() == CallingConv::CDM_INTR),
      IsISRWithContext(IsInterruptHandler && hasOneUsedArgument(F)) {}

MachineFunctionInfo *CDMMachineFunctionInfo::clone(
    BumpPtrAllocator &Allocator, MachineFunction &DestMF,
    const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
    const {
  return DestMF.cloneInfo<CDMMachineFunctionInfo>(*this);
}
