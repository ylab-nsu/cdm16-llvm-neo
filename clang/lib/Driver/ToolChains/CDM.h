#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CDM_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CDM_H

#include "Gnu.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/InputInfo.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY CDMToolChain : public Generic_ELF {
public:
  CDMToolChain(const Driver &D, const llvm::Triple &Triple,
               const llvm::opt::ArgList &Args)
      : Generic_ELF(D, Triple, Args) {}

  const char *getDefaultLinker() const override { return "ld.lld"; }

  bool isPICDefault() const override { return false; }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }

  // We don't have PIE or PIC, so we must ignore all PIC-related flags and
  // always generate non-PIC code
  bool isPICDefaultForced() const override { return true; }

  bool SupportsProfiling() const override { return false; }

protected:
  Tool *buildLinker() const override;
};

} // end namespace toolchains
namespace tools {
namespace CDM {

class LLVM_LIBRARY_VISIBILITY Linker final : public Tool {
public:
  Linker(const ToolChain &TC)
      : Tool("CDM::Linker", "ld.lld", TC) {}

  bool hasIntegratedCPP() const override { return false; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &Args,
                    const char *LinkingOutput) const override;
};

} // end namespace CDM
} // end namespace tools
} // end namespace driver
} // end namespace clang

#endif
