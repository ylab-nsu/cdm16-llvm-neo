#include "CDM.h"
#include "clang/Basic/MacroBuilder.h"

using namespace clang;
using namespace clang::targets;

bool CDMTargetInfo::hasExtension(const StringRef Ext) const {
  const TargetOptions &TargetOpts = getTargetOpts();
  auto Iter = TargetOpts.FeatureMap.find(Ext);
  if (Iter == TargetOpts.FeatureMap.end()) {
    return false;
  }
  return Iter->second;
}

void CDMTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__CDM__");
  if (getTriple().getEnvironment() == llvm::Triple::Cocas) {
    Builder.defineMacro("__COCAS__");
  }
  if (hasExtension("e")) {
    Builder.defineMacro("__CDM_EXT_E__");
  }
  if (hasExtension("m")) {
    Builder.defineMacro("__CDM_EXT_M__");
  }
  Builder.defineMacro("__progmem", "__attribute__((__address_space__(1)))");
}
