//
// Created by ilya on 21.10.23.
//

#include "CDMISelLowering.h"
#include "CDMRegisterInfo.h"
#include "CDMSubtarget.h"
#include "CDMTargetMachine.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetCallingConv.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGenTypes/MachineValueType.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/ErrorHandling.h"
#include <vector>

using namespace llvm;

CDMISelLowering::CDMISelLowering(const CDMTargetMachine &TM,
                                 const CDMSubtarget &ST)
    : TargetLowering(TM), Subtarget(ST) {
  addRegisterClass(MVT::i16, &CDM::CPURegsRegClass);

  computeRegisterProperties(Subtarget.getRegisterInfo());
  setBooleanContents(ZeroOrOneBooleanContent);

  setStackPointerRegisterToSaveRestore(CDM::SP);
  setOperationAction(ISD::STACKSAVE, MVT::Other, Expand);
  setOperationAction(ISD::STACKRESTORE, MVT::Other, Expand);
  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i16, Expand);

  setOperationAction(ISD::SELECT, MVT::i16, Expand);
  setOperationAction(ISD::BRCOND, MVT::Other, Expand);
  setOperationAction(ISD::SETCC, MVT::i16, Expand);

  setOperationAction(ISD::GlobalAddress, MVT::i16, Custom);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);

  // 32-bit and 64-bit shifts are expanded into shift-rotate chains
  // or loops if the shift amount is variable.
  setOperationAction(ISD::SHL, MVT::i32, Custom);
  setOperationAction(ISD::SRL, MVT::i32, Custom);
  setOperationAction(ISD::SRA, MVT::i32, Custom);
  setOperationAction(ISD::SHL, MVT::i64, Custom);
  setOperationAction(ISD::SRL, MVT::i64, Custom);
  setOperationAction(ISD::SRA, MVT::i64, Custom);
  setOperationAction(ISD::SHL_PARTS, MVT::i16, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i16, Expand);
  setOperationAction(ISD::SRA_PARTS, MVT::i16, Expand);

  // Custom lowering
  setOperationAction(ISD::VASTART, MVT::Other, Custom);
  // Default lowering
  setOperationAction(ISD::VAARG, MVT::Other, Custom);
  setOperationAction(ISD::VACOPY, MVT::Other, Custom);
  setOperationAction(ISD::VAEND, MVT::Other, Custom);

  setOperationAction(ISD::BR_JT, MVT::Other, Expand);
  setOperationAction(ISD::JumpTable, MVT::i16, Custom);
}

#include "CDMFunctionInfo.h"
#include "CDMGenCallingConv.inc"

// Mostly taken from llvm-leg
SDValue CDMISelLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  auto &MF = DAG.getMachineFunction();
  auto &MFI = MF.getFrameInfo();
  auto &RegInfo = MF.getRegInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, CC_CDM);

  unsigned RegArgCount = 0;
  for (auto &VA : ArgLocs) {
    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();
      assert(RegVT.getSimpleVT().SimpleTy == MVT::i16 &&
             "Only support 16-bit register passing");
      Register VReg = RegInfo.createVirtualRegister(&CDM::CPURegsRegClass);
      RegInfo.addLiveIn(VA.getLocReg(), VReg);
      SDValue ArgIn = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);
      InVals.push_back(ArgIn);

      RegArgCount++;
      continue;
    }

    EVT ValVT = VA.getValVT();

    // sanity check
    assert(VA.isMemLoc());

    // The stack pointer offset is relative to the caller stack frame.
    int FI =
        MFI.CreateFixedObject(ValVT.getSizeInBits() / 8,
                              4 + StackReserved + VA.getLocMemOffset(), true);
    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(MF.getDataLayout()));

    // Create load nodes to retrieve arguments from the stack
    InVals.push_back(DAG.getLoad(VA.getValVT(), DL, Chain, FIPtr,
                                 MachinePointerInfo::getFixedStack(MF, FI)));
  }

  if (MF.getFunction().hasStructRetAttr()) {
    CDMFunctionInfo *CFI = MF.getInfo<CDMFunctionInfo>();
    Register Reg = CFI->getSRetReturnReg();
    if (!Reg) {
      Reg = MF.getRegInfo().createVirtualRegister(&CDM::CPURegsRegClass);
      CFI->setSRetReturnReg(Reg);
    }
    SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), DL, Reg, InVals[0]);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, Copy, Chain);
  }

  if (!IsVarArg)
    return Chain;

  CDMFunctionInfo *CDMFI = MF.getInfo<CDMFunctionInfo>();
  CDMFI->setVarArgsFrameIndex(0);

  // Block of code below handle varargs passed through registers
  int VaArgStartSPOffset = 4 + RegArgCount * 2;
  SmallVector<SDValue, 5> OutChains;
  for (int VaArgSPOffset = VaArgStartSPOffset, RegNum = 5 + RegArgCount;
       RegNum <= 8; RegNum++, VaArgSPOffset += 2) {
    EVT RegVT = MVT::i16;
    Register VReg = RegInfo.createVirtualRegister(&CDM::CPURegsRegClass);
    RegInfo.addLiveIn(RegNum, VReg);
    SDValue VArg = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);
    int FI = MF.getFrameInfo().CreateFixedObject(2, VaArgSPOffset, true);
    auto PtrVT = getPointerTy(MF.getDataLayout());
    OutChains.push_back(
        DAG.getStore(Chain, DL, VArg, DAG.getFrameIndex(FI, PtrVT),
                     MachinePointerInfo::getFixedStack(MF, FI)));
  }

  auto LastFI = MFI.CreateFixedObject(2, VaArgStartSPOffset, true);
  CDMFI->setVarArgsFrameIndex(LastFI);

  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
  }
  return Chain;
}

SDValue
CDMISelLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                             bool IsVarArg,
                             const SmallVectorImpl<ISD::OutputArg> &Outs,
                             const SmallVectorImpl<SDValue> &OutVals,
                             const SDLoc &DL, SelectionDAG &DAG) const {
  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());

  // In example this loop is in Cpu0TargetLowering::Cpu0CC::analyzeReturn
  // Maybe I should do CCState::AllocateStack
  for (unsigned I = 0, E = Outs.size(); I < E; ++I) {
    MVT VT = Outs[I].VT;
    ISD::ArgFlagsTy Flags = Outs[I].Flags;
    MVT RegVT = MVT::i16;
    if (RetCC_CDM(I, VT, RegVT, CCValAssign::Full, Flags, CCInfo)) {
      dbgs() << "Call result #" << I << " has unhandled type "
             << EVT(VT).getEVTString() << '\n';
      llvm_unreachable("Oops");
    }
  }
  // End of copypaste from other class

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned I = 0; I != RVLocs.size(); ++I) {
    SDValue Val = OutVals[I];
    CCValAssign &VA = RVLocs[I];
    assert(VA.isRegLoc() && "Can only return in registers!");

    if (RVLocs[I].getValVT() != RVLocs[I].getLocVT())
      Val = DAG.getNode(ISD::BITCAST, DL, RVLocs[I].getLocVT(), Val);

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Flag);

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  if (MF.getFunction().hasStructRetAttr()) {
    CDMFunctionInfo *CFI = MF.getInfo<CDMFunctionInfo>();
    Register Reg = CFI->getSRetReturnReg();
    if (!Reg) {
      llvm_unreachable("sret virtual register not created in the entry block");
    }
    auto PtrVT = getPointerTy(DAG.getDataLayout());
    SDValue Val = DAG.getCopyFromReg(Chain, DL, Reg, PtrVT);
    Chain = DAG.getCopyToReg(Chain, DL, CDM::R0, Val);
    RetOps.push_back(DAG.getRegister(CDM::R0, PtrVT));
  }
  //@Ordinary struct type: 2 }

  RetOps[0] = Chain; // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  // Return on Cpu0 is always a "ret $lr"
  return DAG.getNode(CDMISD::Ret, DL, MVT::Other, RetOps);
}

bool CDMISelLowering::CanLowerReturn(
    CallingConv::ID CallingConv, MachineFunction &MF, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context,
    const Type *RetTy) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallingConv, IsVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RetCC_CDM);
}

#define NODE_NAME(x)                                                           \
  case CDMISD::x:                                                              \
    return "CDMISD::" #x
const char *CDMISelLowering::getTargetNodeName(unsigned int Opcode) const {
  switch (Opcode) {
    NODE_NAME(Ret);
    NODE_NAME(Call);
    NODE_NAME(LOAD_SYM);
    NODE_NAME(SHL_EXT32);
    NODE_NAME(SRL_EXT32);
    NODE_NAME(SRA_EXT32);
    NODE_NAME(SHL_EXT64);
    NODE_NAME(SRL_EXT64);
    NODE_NAME(SRA_EXT64);
  default:
    return NULL;
  }
}

SDValue CDMISelLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                   SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &Loc = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  const bool IsVarArg = CLI.IsVarArg;
  MachineFunction &MF = DAG.getMachineFunction();
  const TargetFrameLowering *TFL = MF.getSubtarget().getFrameLowering();

  CLI.IsTailCall = false;

  // Ensure CCInfo is properly declared and initialized
  SmallVector<CCValAssign, 16> ArgLocs; // Declare ArgLocs before CCState usage
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());

  // Analyze operands of the call, assigning locations to each operand.
  CCInfo.AnalyzeCallOperands(Outs, CC_CDM);

  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Create local copies for byval args.
  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned I = 0, E = Outs.size(); I != E; ++I) {
    ISD::ArgFlagsTy Flags = Outs[I].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[I];
    unsigned Size = Flags.getByValSize();
    Align Alignment = Flags.getNonZeroByValAlign();

    if (Size > 0U) {
      int FI = MFI.CreateStackObject(Size, Alignment, false);
      SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      SDValue SizeNode = DAG.getConstant(Size, Loc, MVT::i16);

      Chain = DAG.getMemcpy(Chain, Loc, FIPtr, Arg, SizeNode, Alignment,
                            false,        // isVolatile,
                            (Size <= 16), // AlwaysInline if size <= 16,
                            nullptr,
                            false, // isTailCall
                            MachinePointerInfo(), MachinePointerInfo());
      ByValArgs.push_back(FIPtr);
    } else {
      SDValue NullVal;
      ByValArgs.push_back(NullVal);
    }
  }

  // Get the size of the outgoing arguments stack space requirement.
  unsigned NextStackOffset = CCInfo.getStackSize() + StackReserved;
  unsigned StackAlignment = TFL->getStackAlignment();
  NextStackOffset = alignTo(NextStackOffset, StackAlignment);

  Chain = DAG.getCALLSEQ_START(Chain, NextStackOffset, 0, Loc);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  auto PtrVT = getPointerTy(DAG.getDataLayout());
  SDValue StackPtr = DAG.getCopyFromReg(Chain, Loc, CDM::SP, PtrVT);

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned I = 0, RealArgIdx = 0, ByValArgIdx = 0, E = ArgLocs.size();
       I != E; ++I, ++RealArgIdx) {
    CCValAssign &VA = ArgLocs[I];
    SDValue Arg = OutVals[RealArgIdx];

    ISD::ArgFlagsTy Flags = Outs[RealArgIdx].Flags;
    if (Flags.isByVal()) {
      Arg = ByValArgs[ByValArgIdx++];
      if (!Arg) {
        continue;
      }
    }

    // We only handle fully promoted arguments.
    assert(VA.getLocInfo() == CCValAssign::Full && "Unhandled loc info");

    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
      continue;
    }

    assert(VA.isMemLoc());

    SDValue PtrOff =
        DAG.getIntPtrConstant(VA.getLocMemOffset() + StackReserved, Loc);
    PtrOff = DAG.getNode(ISD::ADD, Loc, PtrVT, StackPtr, PtrOff);

    MemOpChains.push_back(
        DAG.getStore(Chain, Loc, Arg, PtrOff, MachinePointerInfo()));
  }

  // Emit all stores, make sure they occur before the call.
  if (!MemOpChains.empty()) {
    Chain = DAG.getNode(ISD::TokenFactor, Loc, MVT::Other, MemOpChains);
  }

  // Build a sequence of copy-to-reg nodes chained together with token chain
  // and flag operands which copy the outgoing args into the appropriate regs.
  SDValue InFlag;
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, Loc, Reg.first, Reg.second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // We only support calling global addresses.
  PtrVT = getPointerTy(DAG.getDataLayout());

  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), Loc, PtrVT, 0);
  } else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    Callee = DAG.getTargetExternalSymbol(S->getSymbol(), PtrVT, 0);
  }

  std::vector<SDValue> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are known live
  // into the call.
  for (auto &Reg : RegsToPass) {
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
  }

  // Add a register mask operand representing the call-preserved registers.
  const uint32_t *Mask;
  const TargetRegisterInfo *TRI = DAG.getSubtarget().getRegisterInfo();
  Mask = TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv);

  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (InFlag.getNode()) {
    Ops.push_back(InFlag);
  }

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  // Returns a chain and a flag for retval copy to use.
  Chain = DAG.getNode(CDMISD::Call, Loc, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain,
                             DAG.getIntPtrConstant(NextStackOffset, Loc, true),
                             DAG.getIntPtrConstant(0, Loc, true), InFlag, Loc);
  if (!Ins.empty()) {
    InFlag = Chain.getValue(1);
  }

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return lowerCallResult(Chain, InFlag, CallConv, IsVarArg, Ins, Loc, DAG,
                         InVals);
}

SDValue CDMISelLowering::lowerCallResult(
    SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc DL, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const {

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_CDM);

  // Copy all of the result registers out of their specified physreg.
  for (auto &Loc : RVLocs) {
    Chain =
        DAG.getCopyFromReg(Chain, DL, Loc.getLocReg(), Loc.getValVT(), InGlue)
            .getValue(1);
    InGlue = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

void CDMISelLowering::ReplaceNodeResults(SDNode *N,
                                         SmallVectorImpl<SDValue> &Results,
                                         SelectionDAG &DAG) const {
  switch (N->getOpcode()) {
  case ISD::SHL:
  case ISD::SRL:
  case ISD::SRA:
    // Use the default lowering for non-1 constant amount shifts,
    // as it's more efficient.
    if (isa<ConstantSDNode>(N->getOperand(1)) &&
        N->getConstantOperandVal(1) != 1) {
      break;
    }
    [[fallthrough]];
  default:
    SDLoc DL(N);
    SDValue Res = LowerOperation(SDValue(N, 0), DAG);
    for (unsigned I = 0, E = Res->getNumValues(); I != E; ++I)
      Results.push_back(Res.getValue(I));
  }
}

SDValue CDMISelLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::GlobalAddress:
    return lowerGlobalAddress(Op, DAG);
  case ISD::JumpTable:
    return lowerJumpTable(Op, DAG);
  case ISD::VASTART:
    return lowerVASTART(Op, DAG);
  case ISD::SHL:
  case ISD::SRL:
  case ISD::SRA:
    return lowerShifts(Op, DAG);
  }
  return SDValue();
}

SDValue CDMISelLowering::lowerGlobalAddress(SDValue Op,
                                            SelectionDAG &DAG) const {
  EVT VT = Op.getValueType();
  GlobalAddressSDNode *GlobalAddr = cast<GlobalAddressSDNode>(Op.getNode());
  SDValue TargetAddr = DAG.getTargetGlobalAddress(
      GlobalAddr->getGlobal(), Op, MVT::i16, GlobalAddr->getOffset());
  return DAG.getNode(CDMISD::LOAD_SYM, Op, VT, TargetAddr);
}

SDValue CDMISelLowering::lowerJumpTable(SDValue Op, SelectionDAG &DAG) const {
  EVT VT = Op.getValueType();
  JumpTableSDNode *N = cast<JumpTableSDNode>(Op);
  // TODO: check if value type is correct
  SDValue TargetJumpTable = DAG.getTargetJumpTable(N->getIndex(), VT, 0);
  return DAG.getNode(CDMISD::LOAD_SYM, Op, VT, TargetJumpTable);
}

SDValue CDMISelLowering::lowerVASTART(SDValue Op, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();
  CDMFunctionInfo *FuncInfo = MF.getInfo<CDMFunctionInfo>();

  SDLoc DL(Op);
  SDValue FI = DAG.getFrameIndex(FuncInfo->getVarArgsFrameIndex(),
                                 getPointerTy(DAG.getDataLayout()));

  // vastart just stores the address of the VarArgsFrameIndex slot into the
  // memory location argument.
  const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0), DL, FI, Op.getOperand(1),
                      MachinePointerInfo(SV));
}

// Transforms 32-bit and 64-bit shift DAG nodes
// into custom shift_EXT32 and shift_EXT64 nodes.
SDValue CDMISelLowering::lowerShifts(SDValue Op, SelectionDAG &DAG) const {
  const SDNode *N = Op.getNode();
  EVT VT = Op.getValueType();
  SDLoc DL(N);

  int VTSize = VT.getFixedSizeInBits();
  if (VTSize != 32 && VTSize != 64) {
      llvm_unreachable("Unsupported shift value size");
  }

  SmallVector<EVT, 4> ResTypeElements;
  for (int I = 0; I < VTSize / 16; I++) {
    ResTypeElements.push_back(MVT::i16);
  }
  SDVTList ResTys = DAG.getVTList(ResTypeElements);

  SmallVector<SDValue, 5> Operands;
  if (VTSize == 64) {
    auto [Lo32, Hi32] =
        DAG.SplitScalar(Op.getOperand(0), DL, MVT::i32, MVT::i32);
    auto [Op0, Op1] = DAG.SplitScalar(Lo32, DL, MVT::i16, MVT::i16);
    auto [Op2, Op3] = DAG.SplitScalar(Hi32, DL, MVT::i16, MVT::i16);
    Operands.append({Op0, Op1, Op2, Op3});
  } else {
    auto [Lo, Hi] = DAG.SplitScalar(Op.getOperand(0), DL, MVT::i16, MVT::i16);
    Operands.append({Lo, Hi});
  }
  Operands.push_back(Op.getOperand(1));
  unsigned Opc;
  switch (Op.getOpcode()) {
  default:
    llvm_unreachable("Unknown shift operation");
  case ISD::SHL:
    if (VTSize == 64) {
      Opc = CDMISD::SHL_EXT64;
    } else {
      Opc = CDMISD::SHL_EXT32;
    }
    break;
  case ISD::SRL:
    if (VTSize == 64) {
      Opc = CDMISD::SRL_EXT64;
    } else {
      Opc = CDMISD::SRL_EXT32;
    }
    break;
  case ISD::SRA:
    if (VTSize == 64) {
      Opc = CDMISD::SRA_EXT64;
    } else {
      Opc = CDMISD::SRA_EXT32;
    }
    break;
  }
  SDValue Result = DAG.getNode(Opc, DL, ResTys, Operands);
  if (VTSize == 64) {
    SDValue Lo32 = DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i32,
                               Result.getValue(0), Result.getValue(1));
    SDValue Hi32 = DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i32,
                               Result.getValue(2), Result.getValue(3));
    return DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i64, Lo32, Hi32);
  }
  return DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i32, Result.getValue(0),
                     Result.getValue(1));
}

MachineBasicBlock *
CDMISelLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                             MachineBasicBlock *MBB) const {
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("Unknown instruction to emit with custom inserter");
  case CDM::PseudoSelectCC:
  case CDM::PseudoSelectCCI:
    return emitPseudoSelectCC(MI, MBB);
  case CDM::SHL_LARGEAMT:
  case CDM::SHRA_LARGEAMT:
  case CDM::SHR_LARGEAMT:
  case CDM::ROL_LARGEAMT:
  case CDM::ROR_LARGEAMT:
    return emitShiftLargeAmt(MI, MBB);
  case CDM::SHL_LOOP:
  case CDM::SHRA_LOOP:
  case CDM::SHR_LOOP:
  case CDM::ROL_LOOP:
  case CDM::ROR_LOOP:
  case CDM::SHL_EXT32_LOOP:
  case CDM::SHR_EXT32_LOOP:
  case CDM::SHRA_EXT32_LOOP:
  case CDM::SHL_EXT64_LOOP:
  case CDM::SHR_EXT64_LOOP:
  case CDM::SHRA_EXT64_LOOP:
    return emitShiftLoop(MI, MBB);
  }
}

MachineBasicBlock *
CDMISelLowering::emitPseudoSelectCC(MachineInstr &MI,
                                    MachineBasicBlock *MBB) const {
  const CDMInstrInfo &TII =
      *(const CDMInstrInfo *)MBB->getParent()->getSubtarget().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();

  auto Dst = MI.getOperand(0);
  auto LHS = MI.getOperand(1);
  auto RHS = MI.getOperand(2);
  auto TrueVal = MI.getOperand(3);
  auto FalseVal = MI.getOperand(4);
  auto CondCode = static_cast<CDMCOND::CondOp>(MI.getOperand(5).getImm());

  const BasicBlock *BB = MBB->getBasicBlock();

  MachineBasicBlock *HeadMBB = MBB;
  MachineFunction *F = MBB->getParent();
  MachineBasicBlock *TailMBB = F->CreateMachineBasicBlock(BB);
  MachineBasicBlock *IfFalseMBB = F->CreateMachineBasicBlock(BB);

  MachineFunction::iterator I = ++MBB->getIterator();

  F->insert(I, IfFalseMBB);
  F->insert(I, TailMBB);

  TailMBB->splice(TailMBB->begin(), HeadMBB,
                  std::next(MachineBasicBlock::iterator(MI)), HeadMBB->end());

  TailMBB->transferSuccessorsAndUpdatePHIs(HeadMBB);
  HeadMBB->addSuccessor(IfFalseMBB);
  HeadMBB->addSuccessor(TailMBB);
  IfFalseMBB->addSuccessor(TailMBB);

  if (MI.getOpcode() == CDM::PseudoSelectCC) {
    BuildMI(HeadMBB, DL, TII.get(CDM::PseudoBCondRR))
        .addImm(CondCode)
        .addReg(LHS.getReg())
        .addReg(RHS.getReg())
        .addMBB(TailMBB);
  } else {
    BuildMI(HeadMBB, DL, TII.get(CDM::PseudoBCondRI))
        .addImm(CondCode)
        .addReg(LHS.getReg())
        .addImm(RHS.getImm())
        .addMBB(TailMBB);
  }

  BuildMI(*TailMBB, TailMBB->begin(), DL, TII.get(CDM::PHI), Dst.getReg())
      .addReg(TrueVal.getReg())
      .addMBB(HeadMBB)
      .addReg(FalseVal.getReg())
      .addMBB(IfFalseMBB);

  MI.eraseFromParent();

  return TailMBB;
}

MachineBasicBlock *
CDMISelLowering::emitShiftLargeAmt(MachineInstr &MI,
                                   MachineBasicBlock *MBB) const {
  MachineFunction &MF = *MBB->getParent();
  MachineRegisterInfo &RI = MF.getRegInfo();
  const CDMInstrInfo &TII =
      *(const CDMInstrInfo *)MF.getSubtarget().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();

  unsigned Opc;
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("Unknown shift operation");
  case CDM::SHL_LARGEAMT:
    Opc = CDM::SHL;
    break;
  case CDM::SHRA_LARGEAMT:
    Opc = CDM::SHRA;
    break;
  case CDM::SHR_LARGEAMT:
    Opc = CDM::SHR;
    break;
  case CDM::ROL_LARGEAMT:
    Opc = CDM::ROL;
    break;
  case CDM::ROR_LARGEAMT:
    Opc = CDM::ROR;
    break;
  }

  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  int64_t ShAmt = MI.getOperand(2).getImm();
  Register TmpReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);

  BuildMI(*MBB, MI, DL, TII.get(Opc), TmpReg).addReg(SrcReg).addImm(8);
  BuildMI(*MBB, MI, DL, TII.get(Opc), DstReg).addReg(TmpReg).addImm(ShAmt - 8);
  MI.eraseFromParent();

  return MBB;
}

// Replaces shift_LOOP pseudos with inline loops.
MachineBasicBlock *
CDMISelLowering::emitShiftLoop(MachineInstr &MI, MachineBasicBlock *MBB) const {
  MachineFunction &MF = *MBB->getParent();
  MachineRegisterInfo &RI = MF.getRegInfo();
  const CDMInstrInfo &TII =
      *(const CDMInstrInfo *)MF.getSubtarget().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();
  const BasicBlock *BB = MBB->getBasicBlock();

  MachineFunction::iterator I;
  for (I = MBB->getIterator(); I != MF.end() && &(*I) != MBB; ++I)
    ;
  if (I != MF.end())
    ++I;

  unsigned Opc;
  int RegCount;
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("Unknown shift operation");
  case CDM::SHL_LOOP:
    Opc = CDM::SHL;
    RegCount = 1;
    break;
  case CDM::SHRA_LOOP:
    Opc = CDM::SHRA;
    RegCount = 1;
    break;
  case CDM::SHR_LOOP:
    Opc = CDM::SHR;
    RegCount = 1;
    break;
  case CDM::ROL_LOOP:
    Opc = CDM::ROL;
    RegCount = 1;
    break;
  case CDM::ROR_LOOP:
    Opc = CDM::ROR;
    RegCount = 1;
    break;
  case CDM::SHL_EXT32_LOOP:
    Opc = CDM::SHL_EXT32;
    RegCount = 2;
    break;
  case CDM::SHR_EXT32_LOOP:
    Opc = CDM::SHR_EXT32;
    RegCount = 2;
    break;
  case CDM::SHRA_EXT32_LOOP:
    Opc = CDM::SHRA_EXT32;
    RegCount = 2;
    break;
  case CDM::SHL_EXT64_LOOP:
    Opc = CDM::SHL_EXT64;
    RegCount = 4;
    break;
  case CDM::SHR_EXT64_LOOP:
    Opc = CDM::SHR_EXT64;
    RegCount = 4;
    break;
  case CDM::SHRA_EXT64_LOOP:
    Opc = CDM::SHRA_EXT64;
    RegCount = 4;
    break;
  }

  MachineBasicBlock *LoopBB = MF.CreateMachineBasicBlock(BB);
  MachineBasicBlock *CheckBB = MF.CreateMachineBasicBlock(BB);
  MachineBasicBlock *RemBB = MF.CreateMachineBasicBlock(BB);

  MF.insert(I, LoopBB);
  MF.insert(I, CheckBB);
  MF.insert(I, RemBB);

  RemBB->splice(RemBB->begin(), MBB, std::next(MachineBasicBlock::iterator(MI)),
                MBB->end());
  RemBB->transferSuccessorsAndUpdatePHIs(MBB);

  MBB->addSuccessor(CheckBB);
  LoopBB->addSuccessor(CheckBB);
  CheckBB->addSuccessor(LoopBB);
  CheckBB->addSuccessor(RemBB);

  SmallVector<Register, 4> DstRegs;
  SmallVector<Register, 4> SrcRegs;
  for (int OpIndex = 0; OpIndex < RegCount; OpIndex++) {
    DstRegs.push_back(MI.getOperand(OpIndex).getReg());
    SrcRegs.push_back(MI.getOperand(RegCount + OpIndex).getReg());
  }
  Register ShAmtSrcReg = MI.getOperand(RegCount * 2).getReg();

  SmallVector<Register, 4> ShiftRegs;
  SmallVector<Register, 4> ShiftRegs2;
  Register ShiftAmtReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftAmtReg2 = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  for (int RegIndex = 0; RegIndex < RegCount; RegIndex++) {
    ShiftRegs.push_back(RI.createVirtualRegister(&CDM::CPURegsRegClass));
    ShiftRegs2.push_back(RI.createVirtualRegister(&CDM::CPURegsRegClass));
  }

  // MBB:
  // ...
  // br CheckBB
  //
  // LoopBB:
  // ShiftRegs2 = shift ShiftRegs
  // ShiftAmtReg2 = sub ShiftAmtReg, 1
  //
  // CheckBB:
  // ShiftRegs = phi (SrcRegs, MBB), (ShiftRegs2, LoopBB)
  // DstRegs = phi (SrcRegs, MBB), (ShiftRegs2, LoopBB)
  // ShiftAmtReg = phi (ShiftAmtSrcReg, MBB), (ShiftAmtReg2, LoopBB)
  // BCond (ShiftAmtReg > 0), LoopBB
  //
  // RemBB:
  // ...

  BuildMI(MBB, DL, TII.get(CDM::BR)).addMBB(CheckBB);

  auto ShiftOp = BuildMI(LoopBB, DL, TII.get(Opc));
  for (int RegIndex = 0; RegIndex < RegCount; RegIndex++) {
    ShiftOp.addReg(ShiftRegs2[RegIndex], RegState::Define);
  }
  for (int RegIndex = 0; RegIndex < RegCount; RegIndex++) {
    ShiftOp.addReg(ShiftRegs[RegIndex]);
  }
  // Native 16-bit shifts have a constant amount operand,
  // extended shifts don't
  if (RegCount == 1) {
    ShiftOp.addImm(1);
  }
  BuildMI(LoopBB, DL, TII.get(CDM::SUBI), ShiftAmtReg2)
      .addReg(ShiftAmtReg)
      .addImm(1);

  for (int RegIndex = 0; RegIndex < RegCount; RegIndex++) {
    BuildMI(CheckBB, DL, TII.get(CDM::PHI), ShiftRegs[RegIndex])
        .addReg(SrcRegs[RegIndex])
        .addMBB(MBB)
        .addReg(ShiftRegs2[RegIndex])
        .addMBB(LoopBB);
    BuildMI(CheckBB, DL, TII.get(CDM::PHI), DstRegs[RegIndex])
        .addReg(SrcRegs[RegIndex])
        .addMBB(MBB)
        .addReg(ShiftRegs2[RegIndex])
        .addMBB(LoopBB);
  }
  BuildMI(CheckBB, DL, TII.get(CDM::PHI), ShiftAmtReg)
      .addReg(ShAmtSrcReg)
      .addMBB(MBB)
      .addReg(ShiftAmtReg2)
      .addMBB(LoopBB);
  BuildMI(CheckBB, DL, TII.get(CDM::PseudoBCondRI))
      .addImm(CDMCOND::GT)
      .addReg(ShiftAmtReg)
      .addImm(0)
      .addMBB(LoopBB);

  MI.eraseFromParent();

  return RemBB;
}
