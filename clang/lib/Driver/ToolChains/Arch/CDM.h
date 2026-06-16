#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_CDM_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_CDM_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace cdm {

void getCDMTargetFeatures(const Driver &D, const llvm::Triple &Triple,
                          const llvm::opt::ArgList &Args,
                          std::vector<llvm::StringRef> &Features);
} // end namespace cdm
} // end namespace tools
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_CDM_H
