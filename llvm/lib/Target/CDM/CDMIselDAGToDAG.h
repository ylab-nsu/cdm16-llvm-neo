//
// Created by ilya on 21.10.23.
//

#ifndef LLVM_CDMISELDAGTODAG_H
#define LLVM_CDMISELDAGTODAG_H

#include "CDMInstrInfo.h"
#include "CDMTargetMachine.h"

#include "llvm/CodeGen/SelectionDAGISel.h"

namespace llvm {
class CDMDagToDagIsel : public SelectionDAGISel {
public:
  explicit CDMDagToDagIsel(CDMTargetMachine &TM,
                           CodeGenOptLevel OL = CodeGenOptLevel::Default)
      : SelectionDAGISel(TM, OL) {}

private:
#include "CDMGenDAGISel.inc"

  void Select(SDNode *N) override;
  bool trySelectPointerCall(SDNode *N);

  bool selectAddrFrameIndex(SDValue N, SDValue &Addr);
  bool selectAddr(SDValue N, SDValue &Addr);
  bool selectAddr2Reg(SDValue N, SDValue &Base, SDValue &Offset);

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

class CDMDagToDagIselLegacy : public SelectionDAGISelLegacy {
public:
  static char ID;

  StringRef getPassName() const override;

  explicit CDMDagToDagIselLegacy(CDMTargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISelLegacy(
            ID, std::make_unique<CDMDagToDagIsel>(TM, OptLevel)) {}
};

FunctionPass *createCDMISelDagLegacy(CDMTargetMachine &TM,
                                     CodeGenOptLevel OptLevel);

} // namespace llvm

#endif // LLVM_CDMISELDAGTODAG_H
