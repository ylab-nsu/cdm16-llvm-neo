#include "CDM.h"
#include "clang/Basic/MacroBuilder.h"

using namespace clang;
using namespace clang::targets;

void CDMTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__CDM__");
  if (getTriple().getEnvironment() == llvm::Triple::Cocas) {
    Builder.defineMacro("__COCAS__");
  }
  Builder.defineMacro("__progmem", "__attribute__((__address_space__(1)))");
}
