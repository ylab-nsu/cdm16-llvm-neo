#include "CodeGenFunction.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/IR/IntrinsicsCDM.h"

using namespace clang;
using namespace CodeGen;
using namespace llvm;

Value *CodeGenFunction::EmitCDMBuiltinExpr(unsigned BuiltinID,
                                           const CallExpr *E) {
  switch (BuiltinID) {
  default:
    llvm_unreachable("Unexpected CDM builtin");
  case CDM::BI__builtin_cdm_ei: {
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_ei);
    return Builder.CreateCall(Callee);
  }
  case CDM::BI__builtin_cdm_di: {
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_di);
    return Builder.CreateCall(Callee);
  }
  case CDM::BI__builtin_cdm_halt: {
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_halt);
    return Builder.CreateCall(Callee);
  }
  case CDM::BI__builtin_cdm_wait: {
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_wait);
    return Builder.CreateCall(Callee);
  }
  case CDM::BI__builtin_cdm_ldps: {
    llvm::Type *ResultType = ConvertType(E->getType());
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_ldps, ResultType);
    return Builder.CreateCall(Callee);
  }
  case CDM::BI__builtin_cdm_stps: {
    Value *PsValue = {EmitScalarExpr(E->getArg(0))};
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_stps);
    return Builder.CreateCall(Callee, {PsValue});
  }
  case CDM::BI__builtin_cdm_int: {
    Value *Vector = {EmitScalarExpr(E->getArg(0))};
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_int);
    return Builder.CreateCall(Callee, {Vector});
  }
  case CDM::BI__builtin_cdm_reset: {
    Value *Vector = {EmitScalarExpr(E->getArg(0))};
    Function *Callee = CGM.getIntrinsic(Intrinsic::cdm_reset);
    return Builder.CreateCall(Callee, {Vector});
  }
  }
}
