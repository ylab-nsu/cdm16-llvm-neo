#ifndef LLVM_CLANG_SEMA_SEMACDM_H
#define LLVM_CLANG_SEMA_SEMACDM_H

#include "clang/AST/ASTFwd.h"
#include "clang/Sema/SemaBase.h"

namespace clang {
class ParsedAttr;

class SemaCDM : public SemaBase {
public:
  SemaCDM(Sema &S);

  bool CheckCDMBuiltinFunctionCall(unsigned BuiltinID, CallExpr *TheCall);
};
} // namespace clang

#endif // LLVM_CLANG_SEMA_SEMACDM_H
