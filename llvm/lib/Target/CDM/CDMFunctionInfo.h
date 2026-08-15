//
// Created by ilya on 20.11.23.
//

#ifndef LLVM_CDMFUNCTIONINFO_H
#define LLVM_CDMFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class CDMFunctionInfo : public MachineFunctionInfo {
public:
  CDMFunctionInfo(const Function &F, const TargetSubtargetInfo *STI) {
    IsInterruptHandler = F.getCallingConv() == CallingConv::CDM_INTR;
    IsISRWithContext =
        IsInterruptHandler && F.arg_size() == 1 && !F.arg_begin()->use_empty();
  }

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  Register getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(Register Reg) { SRetReturnReg = Reg; }

  // When true, the function saves all registers in the prologue
  // and restores them in the epilogue. This option is used for
  // interrupt service routines with a context pointer parameter.
  bool isISRWithContext() const { return IsISRWithContext; }

  bool isInterruptHandler() { return IsInterruptHandler; }

private:
  virtual void anchor();

  bool IsISRWithContext = false;
  bool IsInterruptHandler = false;
  int VarArgsFrameIndex = 0;
  Register SRetReturnReg;
};

} // namespace llvm

#endif // LLVM_CDMFUNCTIONINFO_H
