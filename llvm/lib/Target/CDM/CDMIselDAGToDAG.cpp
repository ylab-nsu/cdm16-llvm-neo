//
// Created by ilya on 21.10.23.
//

#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "CDM.h"
#include "CDMTargetMachine.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"

#define DEBUG_TYPE "cdm-isel"
#define PASS_NAME "CDM DAG->DAG Instruction Selection"

using namespace llvm;

namespace {

class CDMDAGToDAGISel : public SelectionDAGISel {
public:
  explicit CDMDAGToDAGISel(CDMTargetMachine &TM,
                           CodeGenOptLevel OL = CodeGenOptLevel::Default)
      : SelectionDAGISel(TM, OL) {}

private:
#include "CDMGenDAGISel.inc"
  const CDMSubtarget *Subtarget = nullptr;

  void Select(SDNode *N) override;
  bool trySelectPointerCall(SDNode *N);

  bool selectAddr(SDNode *Op, SDValue N, SDValue &Addr);
  bool selectAddr2Reg(SDNode *Op, SDValue N, SDValue &Base, SDValue &Offset);
  bool selectConstAddr(SDNode *Op, SDValue N, SDValue &Addr);
  bool selectConstAddr2Reg(SDNode *Op, SDValue N, SDValue &Base,
                           SDValue &Offset);
  bool selectAddrFrameIndex(SDNode *Op, SDValue N, SDValue &Addr,
                            SDValue &Offset);

  bool SelectInlineAsmMemoryOperand(const SDValue &Op,
                                    InlineAsm::ConstraintCode ConstraintID,
                                    std::vector<SDValue> &OutOps) override;

  bool runOnMachineFunction(MachineFunction &MF) override {
    Subtarget = &MF.getSubtarget<CDMSubtarget>();
    return SelectionDAGISel::runOnMachineFunction(MF);
  }

  inline CDMCOND::CondOp condCodeToCDMCond(ISD::CondCode CC) const {
    switch (CC) {
    case ISD::CondCode::SETLT:
      return CDMCOND::LT;
    case ISD::CondCode::SETLE:
      return CDMCOND::LE;
    case ISD::CondCode::SETGT:
      return CDMCOND::GT;
    case ISD::CondCode::SETGE:
      return CDMCOND::GE;
    case ISD::CondCode::SETULT:
      return CDMCOND::LO;
    case ISD::CondCode::SETULE:
      return CDMCOND::LS;
    case ISD::CondCode::SETUGT:
      return CDMCOND::HI;
    case ISD::CondCode::SETUGE:
      return CDMCOND::HS;
    case ISD::CondCode::SETEQ:
      return CDMCOND::EQ;
    case ISD::CondCode::SETNE:
      return CDMCOND::NE;
    default:
      llvm_unreachable("Unknown branch condition");
    }
  }
};

class CDMDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
public:
  StringRef getPassName() const override { return PASS_NAME; }

  explicit CDMDAGToDAGISelLegacy(CDMTargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISelLegacy(
            ID, std::make_unique<CDMDAGToDAGISel>(TM, OptLevel)) {}

  static char ID;
};
} // namespace

void CDMDAGToDAGISel::Select(SDNode *N) {

  if (N->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== Tried to select already selected node ";
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

bool CDMDAGToDAGISel::trySelectPointerCall(SDNode *N) {
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

bool CDMDAGToDAGISel::selectAddr(SDNode *Op, SDValue N, SDValue &Base) {
  if (isProgramMemoryAccess(cast<MemSDNode>(Op))) {
    return false;
  }
  if (isa<FrameIndexSDNode>(N)) {
    return false;
  }
  if (N.getOpcode() == ISD::ADD) {
    return false;
  }
  Base = N;
  return true;
}

bool CDMDAGToDAGISel::selectAddr2Reg(SDNode *Op, SDValue N, SDValue &Base,
                                     SDValue &Offset) {
  if (isProgramMemoryAccess(cast<MemSDNode>(Op))) {
    return false;
  }

  if (N.getOpcode() == ISD::ADD) {
    const SDValue &Op1 = N.getOperand(0);
    const SDValue &Op2 = N.getOperand(1);

    if (isa<FrameIndexSDNode>(Op1) && isa<ConstantSDNode>(Op2)) {
      return false;
    } else if (isa<FrameIndexSDNode>(Op2) && isa<ConstantSDNode>(Op1)) {
      return false;
    }

    Base = Op1;
    Offset = Op2;
    return true;
  }
  return false;
}

bool CDMDAGToDAGISel::selectAddrFrameIndex(SDNode *Op, SDValue N, SDValue &Base,
                                           SDValue &Offset) {
  EVT ValTy = N.getValueType();
  SDLoc DL(N);

  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N)) {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
    Offset = CurDAG->getTargetConstant(0, DL, ValTy);
    return true;
  }
  if (N->getOpcode() == ISD::ADD) {
    const SDValue &Op1 = N.getOperand(0);
    const SDValue &Op2 = N.getOperand(1);

    if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Op1)) {
      if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Op2)) {
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
        Offset = CurDAG->getTargetConstant(CN->getAPIntValue(), DL, ValTy);
        return true;
      }
    } else if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Op2)) {
      if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Op1)) {
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
        Offset = CurDAG->getTargetConstant(CN->getAPIntValue(), DL, ValTy);
        return true;
      }
    }
  }
  return false;
}

bool CDMDAGToDAGISel::selectConstAddr(SDNode *Op, SDValue N, SDValue &Base) {
  if (!isProgramMemoryAccess(cast<MemSDNode>(Op))) {
    return false;
  }
  if (isa<FrameIndexSDNode>(N)) {
    return false;
  }
  if (N.getOpcode() == ISD::ADD) {
    return false;
  }
  Base = N;
  return true;
}

bool CDMDAGToDAGISel::selectConstAddr2Reg(SDNode *Op, SDValue N, SDValue &Base,
                                          SDValue &Offset) {
  if (!isProgramMemoryAccess(cast<MemSDNode>(Op))) {
    return false;
  }

  if (N.getOpcode() == ISD::ADD) {
    Base = N.getOperand(0);
    Offset = N.getOperand(1);
    return true;
  }
  return false;
}

bool CDMDAGToDAGISel::SelectInlineAsmMemoryOperand(
    const SDValue &Op, InlineAsm::ConstraintCode ConstraintCode,
    std::vector<SDValue> &OutOps) {
  switch (ConstraintCode) {
  case InlineAsm::ConstraintCode::m:
  case InlineAsm::ConstraintCode::o:
    break;
  default:
    return true;
  }
  OutOps.push_back(Op);
  return false;
}

FunctionPass *llvm::createCDMISelDag(llvm::CDMTargetMachine &TM,
                                     CodeGenOptLevel OptLevel) {
  return new CDMDAGToDAGISelLegacy(TM, OptLevel);
}

char CDMDAGToDAGISelLegacy::ID = 0;

INITIALIZE_PASS(CDMDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)
