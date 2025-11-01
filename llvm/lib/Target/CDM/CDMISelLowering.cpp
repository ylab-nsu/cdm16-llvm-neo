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

  //          setOperationAction(ISD::BR_CC, MVT::i16, Expand);

  //          setOperationAction(ISD::SELECT_CC, MVT::i16, Expand);
  setOperationAction(ISD::SELECT, MVT::i16, Expand);
  setOperationAction(ISD::BRCOND, MVT::Other, Expand);
  setOperationAction(ISD::SETCC, MVT::i16, Expand);
  //          setOperationAction(ISD::SELECT_CC, MVT::i16, Custom);

  setOperationAction(ISD::GlobalAddress, MVT::i16, Custom);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);

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
SDValue CDMISelLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::GlobalAddress:
    return lowerGlobalAddress(Op, DAG);
  case ISD::JumpTable:
    return lowerJumpTable(Op, DAG);
  case ISD::VASTART:
    return lowerVASTART(Op, DAG);
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

MachineBasicBlock *CDMISelLowering::insertShiftVarAmt(MachineInstr &MI,
                                                MachineBasicBlock *BB) const {
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RI = MF->getRegInfo();
  const CDMInstrInfo &TII =
      *(const CDMInstrInfo *)BB->getParent()->getSubtarget().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();
  const BasicBlock *LLVM_BB = BB->getBasicBlock();

  MachineFunction::iterator I;
  for (I = BB->getIterator(); I != MF->end() && &(*I) != BB; ++I)
    ;
  if (I != MF->end())
    ++I;

  unsigned Opc;
  switch (MI.getOpcode()) {
  case CDM::SHL_VARAMT:
    Opc = CDM::SHL;
    break;
  case CDM::SHRA_VARAMT:
    Opc = CDM::SHRA;
    break;
  case CDM::SHR_VARAMT:
    Opc = CDM::SHR;
    break;
  case CDM::ROL_VARAMT:
    Opc = CDM::ROL;
    break;
  case CDM::ROR_VARAMT:
    Opc = CDM::ROR;
    break;
  default:
    llvm_unreachable("Unknown shift operation");
  }

  MachineBasicBlock *LoopBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *CheckBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *RemBB = MF->CreateMachineBasicBlock(LLVM_BB);

  MF->insert(I, LoopBB);
  MF->insert(I, CheckBB);
  MF->insert(I, RemBB);

  // Update machine-CFG edges by transferring all successors of the current
  // block to the block containing instructions after shift.
  RemBB->splice(RemBB->begin(), BB, std::next(MachineBasicBlock::iterator(MI)),
                BB->end());
  RemBB->transferSuccessorsAndUpdatePHIs(BB);

  // Add edges BB => LoopBB => CheckBB => RemBB, CheckBB => LoopBB.
  BB->addSuccessor(CheckBB);
  LoopBB->addSuccessor(CheckBB);
  CheckBB->addSuccessor(LoopBB);
  CheckBB->addSuccessor(RemBB);

  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  Register ShAmtSrcReg = MI.getOperand(2).getReg();

  Register ShiftReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftReg2 = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftAmtReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftAmtReg2 = RI.createVirtualRegister(&CDM::CPURegsRegClass);

  BuildMI(BB, DL, TII.get(CDM::BR)).addMBB(CheckBB);

  BuildMI(LoopBB, DL, TII.get(Opc), ShiftReg2).addReg(ShiftReg).addImm(1);

  BuildMI(CheckBB, DL, TII.get(CDM::PHI), ShiftReg)
      .addReg(SrcReg)
      .addMBB(BB)
      .addReg(ShiftReg2)
      .addMBB(LoopBB);
  BuildMI(CheckBB, DL, TII.get(CDM::PHI), ShiftAmtReg)
      .addReg(ShAmtSrcReg)
      .addMBB(BB)
      .addReg(ShiftAmtReg2)
      .addMBB(LoopBB);
  BuildMI(CheckBB, DL, TII.get(CDM::PHI), DstReg)
      .addReg(SrcReg)
      .addMBB(BB)
      .addReg(ShiftReg2)
      .addMBB(LoopBB);

  BuildMI(CheckBB, DL, TII.get(CDM::SUBI), ShiftAmtReg2)
      .addReg(ShiftAmtReg)
      .addImm(1);
  BuildMI(CheckBB, DL, TII.get(CDM::PseudoBCondRI))
      .addImm(CDMCOND::GT)
      .addReg(ShiftAmtReg2)
      .addImm(0)
      .addMBB(LoopBB);

  MI.eraseFromParent();

  return RemBB;
}

MachineBasicBlock *CDMISelLowering::insertShiftParts(MachineInstr &MI,
                                                MachineBasicBlock *BB) const {
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RI = MF->getRegInfo();
  const CDMInstrInfo &TII =
      *(const CDMInstrInfo *)BB->getParent()->getSubtarget().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();
  const BasicBlock *LLVM_BB = BB->getBasicBlock();

  MachineFunction::iterator I;
  for (I = BB->getIterator(); I != MF->end() && &(*I) != BB; ++I)
    ;
  if (I != MF->end())
    ++I;

  unsigned Opc;
  switch (MI.getOpcode()) {
  case CDM::SHL_PARTS:
    Opc = CDM::SHL_PARTS_1;
    break;
  case CDM::SRL_PARTS:
    Opc = CDM::SRL_PARTS_1;
    break;
  case CDM::SRA_PARTS:
    Opc = CDM::SRA_PARTS_1;
    break;
  default:
    llvm_unreachable("Unknown shift operation");
  }

  MachineBasicBlock *LoopBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *CheckBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *RemBB = MF->CreateMachineBasicBlock(LLVM_BB);

  MF->insert(I, LoopBB);
  MF->insert(I, CheckBB);
  MF->insert(I, RemBB);

  // Update machine-CFG edges by transferring all successors of the current
  // block to the block containing instructions after shift.
  RemBB->splice(RemBB->begin(), BB, std::next(MachineBasicBlock::iterator(MI)),
                BB->end());
  RemBB->transferSuccessorsAndUpdatePHIs(BB);

  // Add edges BB => LoopBB => CheckBB => RemBB, CheckBB => LoopBB.
  BB->addSuccessor(CheckBB);
  LoopBB->addSuccessor(CheckBB);
  CheckBB->addSuccessor(LoopBB);
  CheckBB->addSuccessor(RemBB);

  Register DstLoReg = MI.getOperand(0).getReg();
  Register DstHiReg = MI.getOperand(1).getReg();
  Register SrcLoReg = MI.getOperand(2).getReg();
  Register SrcHiReg = MI.getOperand(3).getReg();
  Register ShAmtSrcReg = MI.getOperand(4).getReg();

  Register ShiftLoReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftHiReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftLoReg2 = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftHiReg2 = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftAmtReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);
  Register ShiftAmtReg2 = RI.createVirtualRegister(&CDM::CPURegsRegClass);

  BuildMI(BB, DL, TII.get(CDM::BR)).addMBB(CheckBB);

  BuildMI(LoopBB, DL, TII.get(Opc))
      .addReg(ShiftLoReg2, RegState::Define)
      .addReg(ShiftHiReg2, RegState::Define)
      .addReg(ShiftLoReg)
      .addReg(ShiftHiReg);

  BuildMI(CheckBB, DL, TII.get(CDM::PHI), ShiftLoReg)
      .addReg(SrcLoReg)
      .addMBB(BB)
      .addReg(ShiftLoReg2)
      .addMBB(LoopBB);
  BuildMI(CheckBB, DL, TII.get(CDM::PHI), ShiftHiReg)
      .addReg(SrcHiReg)
      .addMBB(BB)
      .addReg(ShiftHiReg2)
      .addMBB(LoopBB);
  BuildMI(CheckBB, DL, TII.get(CDM::PHI), ShiftAmtReg)
      .addReg(ShAmtSrcReg)
      .addMBB(BB)
      .addReg(ShiftAmtReg2)
      .addMBB(LoopBB);
  BuildMI(CheckBB, DL, TII.get(CDM::PHI), DstLoReg)
      .addReg(SrcLoReg)
      .addMBB(BB)
      .addReg(ShiftLoReg2)
      .addMBB(LoopBB);
  BuildMI(CheckBB, DL, TII.get(CDM::PHI), DstHiReg)
      .addReg(SrcHiReg)
      .addMBB(BB)
      .addReg(ShiftHiReg2)
      .addMBB(LoopBB);

  BuildMI(CheckBB, DL, TII.get(CDM::SUBI), ShiftAmtReg2)
      .addReg(ShiftAmtReg)
      .addImm(1);
  BuildMI(CheckBB, DL, TII.get(CDM::PseudoBCondRI))
      .addImm(CDMCOND::GT)
      .addReg(ShiftAmtReg2)
      .addImm(0)
      .addMBB(LoopBB);

  MI.eraseFromParent();

  return RemBB;
}

MachineBasicBlock *CDMISelLowering::insertShiftLargeAmt(MachineInstr &MI,
                                                MachineBasicBlock *MBB) const {
  MachineFunction *MF = MBB->getParent();
  MachineRegisterInfo &RI = MF->getRegInfo();
  const CDMInstrInfo &TII =
      *(const CDMInstrInfo *)MBB->getParent()->getSubtarget().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();

  unsigned Opc;
  switch (MI.getOpcode()) {
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
  default:
    assert(false && "Unknown shift operation");
  }

  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  int64_t ShAmt = MI.getOperand(2).getImm();
  Register TmpReg = RI.createVirtualRegister(&CDM::CPURegsRegClass);

  BuildMI(*MBB, MI.getIterator(), DL, TII.get(Opc), TmpReg).addReg(SrcReg).addImm(8);
  BuildMI(*MBB, MI.getIterator(), DL, TII.get(Opc), DstReg).addReg(TmpReg).addImm(ShAmt - 8);
  MI.eraseFromParent();

  return MBB;
}

MachineBasicBlock *
CDMISelLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                             MachineBasicBlock *MBB) const {
  switch (MI.getOpcode()){
	  default:
		llvm_unreachable("Unexpected instr type to insert");
	  case CDM::PseudoSelectCC:
	  case CDM::PseudoSelectCCI:
		return emitPseudoSelectCC(MI, MBB);
      case CDM::SHL_LARGEAMT:
      case CDM::SHRA_LARGEAMT:
      case CDM::SHR_LARGEAMT:
      case CDM::ROL_LARGEAMT:
      case CDM::ROR_LARGEAMT:
        return insertShiftLargeAmt(MI, MBB);
      case CDM::SHL_VARAMT:
      case CDM::SHRA_VARAMT:
      case CDM::SHR_VARAMT:
      case CDM::ROL_VARAMT:
      case CDM::ROR_VARAMT:
        return insertShiftVarAmt(MI, MBB);
      case CDM::SHL_PARTS:
      case CDM::SRL_PARTS:
      case CDM::SRA_PARTS:
        return insertShiftParts(MI, MBB);
  }
}

MachineBasicBlock*
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

  const BasicBlock *LLVM_BB = MBB->getBasicBlock();

  MachineBasicBlock *HeadMBB = MBB;
  MachineFunction *F = MBB->getParent();
  MachineBasicBlock *TailMBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *IfFalseMBB = F->CreateMachineBasicBlock(LLVM_BB);

  MachineFunction::iterator I = ++MBB->getIterator();

  F->insert(I, IfFalseMBB);
  F->insert(I, TailMBB);

  TailMBB->splice(TailMBB->begin(), HeadMBB,
                  std::next(MachineBasicBlock::iterator(MI)), HeadMBB->end());

  TailMBB->transferSuccessorsAndUpdatePHIs(HeadMBB);
  HeadMBB->addSuccessor(IfFalseMBB);
  HeadMBB->addSuccessor(TailMBB);
  IfFalseMBB->addSuccessor(TailMBB);

  if (MI.getOpcode() == CDM::PseudoSelectCC){
	  BuildMI(HeadMBB, DL, TII.get(CDM::PseudoBCondRR))
					.addImm(CondCode)
					.addReg(LHS.getReg())
					.addReg(RHS.getReg())
					.addMBB(TailMBB);
  }
  else{
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
