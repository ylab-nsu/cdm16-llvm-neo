//
// Created by ilya on 21.10.23.
//

#include "CDMIselDAGToDAG.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <utility>

using namespace llvm;

#define DEBUG_TYPE "cdm-isel"
#define PASS_NAME "CDM DAG->DAG Bullshit Instruction Selection"

char CDMDagToDagIselLegacy::ID = 0;

StringRef CDMDagToDagIselLegacy::getPassName() const { return PASS_NAME; }

void CDMDagToDagIsel::Select(SDNode *N) {

  if (N->isMachineOpcode()) {
    LLVM_DEBUG(
        errs() << "== Something fucked up; selecting already selected node";
        N->dump(CurDAG); errs() << "\n");
    N->setNodeId(-1);
    return;
  }

  if (N->getOpcode() == CDMISD::CALL) {
    // Generate JSRR if needed, otherwise fall to
    // default tablegen pattern matching
    if (trySelectPointerCall(N))
      return;
  }

  SelectCode(N);
}

bool CDMDagToDagIsel::runOnMachineFunction(MachineFunction &MF) {
  return SelectionDAGISel::runOnMachineFunction(MF);
}

bool CDMDagToDagIsel::trySelect(SDNode *Node) {
  return false; // TODO: actually select
}

bool CDMDagToDagIsel::SelectAddr(SDValue N, SDValue &Base) {
  if (isa<FrameIndexSDNode>(N)) {
    return false;
  }

  Base = N;
  return true;
}

bool CDMDagToDagIsel::SelectAddr2Reg(SDValue N, SDValue &Base,
                                     SDValue &Offset) {
  if (N.getOpcode() == ISD::ADD) {
    Base = N.getOperand(0);
    Offset = N.getOperand(1);
    return true;
  }

  return false;
}

bool CDMDagToDagIsel::SelectAddrFrameIndex(SDValue N, SDValue &Base) {
  EVT ValTy = N.getValueType();
  SDLoc DL(N);

  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N)) {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
    return true;
  }

  // Cant load not from stack yet
  LLVM_DEBUG(errs() << "[LEADP] Cant select frame address\n");
  return false;
}

bool CDMDagToDagIsel::trySelectPointerCall(SDNode *N) {
  SDValue Target = N->getOperand(1);

  // If target is just symbol, pass through to default tablegen pattern matching
  if (dyn_cast<GlobalAddressSDNode>(Target) != nullptr ||
      dyn_cast<ExternalSymbolSDNode>(Target) != nullptr)
    return false;

  // Layout of CDMCall node's operands is:
  // [Chain, Target, <Arg0>, ..., Mask, <Glue>]
  //
  // Layout for JSRR/JSR needs to be:
  // [Target, <Arg0>, ..., Mask, Chain, <Glue>]

  SmallVector<SDValue> Operands = {Target};

  auto *const ArgsBegin = N->op_begin() + 2;
  auto *const ArgsEnd =
      std::find_if(ArgsBegin, N->op_end(), [](const SDUse &UseOp) {
        return static_cast<ISD::NodeType>(UseOp.get().getOpcode()) ==
               ISD::RegisterMask;
      });

  if (ArgsEnd == N->op_end())
    llvm_unreachable("CDMCall Node missing RegisterMask, can't find end of "
                     "argument operands");

  std::copy(ArgsBegin, ArgsEnd, std::back_inserter(Operands));

  SDValue Mask = *ArgsEnd;
  Operands.push_back(std::move(Mask));

  SDValue Chain = N->getOperand(0);
  Operands.push_back(Chain);

  // The last operand (Glue) is not always necessary, so we check if we are at
  // the end yet
  if ((ArgsEnd + 1) < N->op_end()) {
    SDValue Glue = *(ArgsEnd + 1);
    Operands.push_back(std::move(Glue));
  }

  CurDAG->SelectNodeTo(N, CDM::JSRR, MVT::Other, MVT::Glue, Operands);

  return true;
}

FunctionPass *llvm::createCDMISelDagLegacy(llvm::CDMTargetMachine &TM,
                                           CodeGenOptLevel OptLevel) {
  return new CDMDagToDagIselLegacy(TM, OptLevel);
}
