//
// Created by ilya on 21.10.23.
//

#ifndef LLVM_CDMISELLOWERING_H
#define LLVM_CDMISELLOWERING_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/IR/Function.h"

namespace llvm {
namespace CDMISD {
enum NodeType {
  // Start the numbering from where ISD NodeType finishes.
  FIRST_NUMBER = ISD::BUILTIN_OP_END,

  // Return
  Ret,

  // Call
  Call,

  LOAD_SYM,
};
} // namespace CDMISD

class CDMTargetMachine;
class CDMSubtarget;
class CDMISelLowering : public TargetLowering {
public:
  explicit CDMISelLowering(const CDMTargetMachine &TM, const CDMSubtarget &ST);
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;
  bool CanLowerReturn(CallingConv::ID CallingConv, MachineFunction &MF,
                      bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      LLVMContext &Context, const Type *RetTy) const override;
  const char *getTargetNodeName(unsigned int Opcode) const override;
  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *MBB) const override;

private:
  SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerJumpTable(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerCallResult(SDValue Chain, SDValue InGlue,
                          CallingConv::ID CallConv, bool IsVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc DL,
                          SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;
  SDValue lowerVASTART(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerVAARG(SDValue Op, SelectionDAG &DAG);
  
  MachineBasicBlock *insertPseudoSelect(MachineInstr &MI, MachineBasicBlock *MBB) const;
  MachineBasicBlock *insertShiftLargeAmt(MachineInstr &MI, MachineBasicBlock *MBB) const;
  MachineBasicBlock *insertShiftVarAmt(MachineInstr &MI, MachineBasicBlock *MBB) const;

  MachineBasicBlock* emitPseudoSelectCC(MachineInstr &MI,
		  			MachineBasicBlock *MBB) const;

  const CDMSubtarget &Subtarget;
  const unsigned StackReserved = 4 * 2u;
};

} // namespace llvm

#endif // LLVM_CDMISELLOWERING_H
