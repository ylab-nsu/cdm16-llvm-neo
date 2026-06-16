#ifndef LLVM_TARGETPARSER_CDMTARGETPARSER_H
#define LLVM_TARGETPARSER_CDMTARGETPARSER_H

#include "llvm/Support/Compiler.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace llvm {
class StringRef;

namespace CDM {

struct ExtensionInfo {
  char ArchName;
  StringRef FlagName;
};

LLVM_ABI bool getArchFeatures(StringRef Arch, std::vector<StringRef> &Features);

} // namespace CDM

} // namespace llvm

#endif // LLVM_TARGETPARSER_CDMTARGETPARSER_H
