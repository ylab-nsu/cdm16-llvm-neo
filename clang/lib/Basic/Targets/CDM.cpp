#include "CDM.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

using namespace clang;
using namespace clang::targets;

#define GET_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsCDM.inc"
#undef GET_BUILTIN_STR_TABLE

static constexpr Builtin::Info BuiltinInfos[] = {
#define GET_BUILTIN_INFOS
#include "clang/Basic/BuiltinsCDM.inc"
#undef GET_BUILTIN_INFOS
};

llvm::SmallVector<Builtin::InfosShard>
CDMTargetInfo::getTargetBuiltins() const {
  return {
      {&BuiltinStrings, BuiltinInfos},
  };
}

void CDMTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__CDM__");
  if (getTriple().getEnvironment() == llvm::Triple::Cocas) {
    Builder.defineMacro("__COCAS__");
  }
}
