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
    IsInterruptHandler = F.getCallingConv() == CallingConv::CdmIsr;
  }

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  Register getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(Register Reg) { SRetReturnReg = Reg; }

  bool isInterruptHandler() { return IsInterruptHandler; }

private:
  virtual void anchor();

  bool IsInterruptHandler;
  int VarArgsFrameIndex = 0;
  Register SRetReturnReg;
};

} // namespace llvm

#endif // LLVM_CDMFUNCTIONINFO_H
