#include "llvm/TargetParser/CDMTargetParser.h"

using namespace llvm;
using namespace llvm::CDM;

const ExtensionInfo AllExtensions[] = {
#define CDM_EXTENSION(ARCH_NAME, FLAG_NAME) {ARCH_NAME, FLAG_NAME},
#include "llvm/TargetParser/CDMTargetParser.def"
};

bool CDM::getArchFeatures(StringRef Arch, std::vector<StringRef> &Features) {
  if (!Arch.starts_with("cdm16")) {
    return false;
  }
  for (const char &Ch : Arch.slice(5, -1)) {
    bool Valid = false;
    for (const auto &Ext : AllExtensions) {
      if (Ext.ArchName == Ch) {
        Features.push_back(Ext.FlagName);
        Valid = true;
        break;
      }
    }
    if (!Valid) {
      return false;
    }
  }
  return true;
}
