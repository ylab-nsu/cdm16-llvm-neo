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
  case CDM::BI__builtin_cdm_int_enable:
  case CDM::BI__builtin_cdm_int_disable:
  case CDM::BI__builtin_cdm_halt:
  case CDM::BI__builtin_cdm_wait:
  case CDM::BI__builtin_cdm_get_ps:
  case CDM::BI__builtin_cdm_set_ps: {
    return false;
  }
  case CDM::BI__builtin_cdm_interrupt:
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
