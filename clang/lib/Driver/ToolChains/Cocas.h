#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_COCAS_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_COCAS_H

#include "clang/Driver/Action.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/InputInfo.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include <optional>
#include <vector>

namespace clang {
namespace driver {

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY CocasToolChain : public ToolChain {

  std::optional<std::string> CocasPath; // Cocas executable path
  std::optional<std::string> IncludePath;
  // TODO: Add standard lib object files
  const std::vector<const char *> StdLibObjs = {};
  // TODO: Add names of builtins to link
  // Each name <name> corresponds to file
  // <clang_resources_dir>/lib/cdm/clang_rt.<name>.o
  const std::vector<const char *> BuiltinNames = {};

public:
  CocasToolChain(const Driver &D, const llvm::Triple &Triple,
               const llvm::opt::ArgList &Args);

  // CdM toolchain doesn't have integrated assembler, it must use cocas
  bool useIntegratedAs() const override { return false; }

  bool isPICDefault() const override { return false; }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }

  // We don't have PIE or PIC, so we must ignore all PIC-related flags and always
  // generate non-PIC code
  bool isPICDefaultForced() const override { return true; }

  llvm::opt::DerivedArgList *
  TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
                Action::OffloadKind DeviceOffloadKind) const override;

  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;

  bool SupportsProfiling() const override { return false; }

  std::optional<std::string> getCocasPath() const { return CocasPath; }
  std::optional<std::string> getIncludePath() const { return IncludePath; }
  const std::vector<const char *> &getStdLibObjs() const { return StdLibObjs; }
  const std::vector<const char *> &getBuiltinNames() const {
    return BuiltinNames;
  }

protected:
  Tool *buildAssembler() const override; // cocas
  Tool *buildLinker() const override;    // cocas
};

} // end namespace toolchains

namespace tools {
namespace CDM {

class LLVM_LIBRARY_VISIBILITY Cocas final : public Tool {
public:
  Cocas(const toolchains::CocasToolChain &TC);

  bool hasIntegratedCPP() const override { return false; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &Args,
                    const char *LinkingOutput) const override;

  const toolchains::CocasToolChain &getCocasToolChain() const {
    return static_cast<const toolchains::CocasToolChain &>(getToolChain());
  }
};

} // end namespace CDM
} // end namespace tools
} // end namespace driver
} // end namespace clang

#endif
