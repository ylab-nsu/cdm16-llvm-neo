#include "clang/Sema/SemaCDM.h"
#include "clang/AST/DeclBase.h"
#include "clang/Basic/DiagnosticSema.h"
#include "clang/Sema/Attr.h"
#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"

namespace clang {
SemaCDM::SemaCDM(Sema &S) : SemaBase(S) {}

void SemaCDM::handleInterruptAttr(Decl *D, const ParsedAttr &AL) {
  if (!isFuncOrMethodForAttrSubject(D) || !hasFunctionProto(D) ||
      isInstanceMethod(D) ||
      CXXMethodDecl::isStaticOverloadedOperator(
          cast<NamedDecl>(D)->getDeclName().getCXXOverloadedOperator())) {
    Diag(AL.getLoc(), diag::warn_attribute_wrong_decl_type)
        << AL << AL.isRegularKeywordAttribute()
        << ExpectedFunctionWithProtoType;
    return;
  }

  if (!AL.checkExactlyNumArgs(SemaRef, 0)) {
    return;
  }

  // CDM interrupt handlers must have one pointer parameter
  if (getFunctionOrMethodNumParams(D) != 1) {
    Diag(D->getLocation(), diag::warn_interrupt_signal_attribute_invalid)
        << /*CDM*/ 4 << /*interrupt*/ 0 << 2;
    return;
  }

  QualType ParamType = getFunctionOrMethodParamType(D, 0);
  if (!ParamType->isPointerType()) {
    Diag(D->getLocation(), diag::warn_interrupt_signal_attribute_invalid)
        << /*CDM*/ 4 << /*interrupt*/ 0 << 2;
    return;
  }

  // CDM interrupt handlers must return void
  if (!getFunctionOrMethodResultType(D)->isVoidType()) {
    Diag(D->getLocation(), diag::warn_interrupt_signal_attribute_invalid)
        << /*CDM*/ 4 << /*interrupt*/ 0 << 1;
    return;
  }

  handleSimpleAttribute<CDMInterruptAttr>(*this, D, AL);
}

} // namespace clang
