//
// Created by ilya on 20.11.23.
//

#ifndef LLVM_CDMFUNCTIONINFO_H
#define LLVM_CDMFUNCTIONINFO_H

#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class CDMMachineFunctionInfo : public MachineFunctionInfo {
public:
  CDMMachineFunctionInfo(const Function &F, const TargetSubtargetInfo *STI);

  MachineFunctionInfo *
  clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
        const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
      const override;

  // Whether the function has the CDM interrupt calling convention.
  bool isInterruptHandler() { return IsInterruptHandler; }

  // When true, the function saves all registers in the prologue
  // and restores them in the epilogue. The function must be an ISR
  // with a single pointer parameter that points the context saved on the stack.
  bool isISRWithContext() const { return IsISRWithContext; }

  // This flag is set in the CDM frame analyzer pass and forces PEI to enable
  // FP, since all frame indices are lowered to FP-relative instructions.
  bool getUsesFrameIndices() const { return UsesFrameIndices; }
  void setUsesFrameIndices(bool Value) { UsesFrameIndices = Value; }

  // The size of callee-saved registers on the stack. It is added to each frame
  // offset because CSRs below FP are counted as part of the frame.
  unsigned getCaleeSavedSize() const { return CalleeSavedSize; }
  void setCaleeSavedSize(unsigned Value) { CalleeSavedSize = Value; }

  // This size is used as a heuristic for enabling the frame pointer
  // and using the alternative SP adjustment algorithm.
  //
  // It is calculated in the CDM frame analyzer pass before PEI, so it doesn't
  // include this size of the callee saves. They are located below FP on the
  // stack, so they don't need to be accounted for when doing stack adjustment.
  unsigned getEstimatedFrameSize() const { return EstimatedFrameSize; }
  void setEstimatedFrameSize(unsigned Value) { EstimatedFrameSize = Value; }

  // The frame index of the 8-byte space used for spilling the initial arguments
  // passed in r0..r3 when using varargs.
  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  // The virtual register used for storing the result pointer passed as an
  // implicit first argument (in r0) to the function when it has a non-scalar
  // (e.g. struct) return type.
  //
  // The result pointer can not be left in r0, bacuse it may get clobbered.
  Register getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(Register Reg) { SRetReturnReg = Reg; }

private:
  bool IsInterruptHandler = false;
  bool IsISRWithContext = false;
  bool UsesFrameIndices = false;
  unsigned CalleeSavedSize = 0;
  unsigned EstimatedFrameSize = 0;
  int VarArgsFrameIndex = 0;
  Register SRetReturnReg = CDM::NoRegister;
};

} // namespace llvm

#endif // LLVM_CDMFUNCTIONINFO_H
