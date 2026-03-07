#include "clang/Sema/SemaCDM.h"
#include "clang/Basic/DiagnosticSema.h"
#include "clang/Basic/TargetBuiltins.h"
#include "clang/Sema/Sema.h"

namespace clang {

SemaCDM::SemaCDM(Sema &S) : SemaBase(S) {}

bool SemaCDM::CheckCDMBuiltinFunctionCall(unsigned BuiltinID,
                                          CallExpr *TheCall) {
  switch (BuiltinID) {
  default:
    llvm_unreachable("Unexpected CDM builtin");
  case CDM::BI__builtin_cdm_ei:
  case CDM::BI__builtin_cdm_di:
  case CDM::BI__builtin_cdm_halt:
  case CDM::BI__builtin_cdm_wait:
  case CDM::BI__builtin_cdm_ldps:
  case CDM::BI__builtin_cdm_stps: {
    return false;
  }
  case CDM::BI__builtin_cdm_int:
  case CDM::BI__builtin_cdm_reset: {
    if (SemaRef.checkArgCount(TheCall, 1))
      return true;
    if (SemaRef.BuiltinConstantArgRange(TheCall, 0, 0, 511))
      return true;
    return false;
  }
  }
}
} // namespace clang
