#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CDM_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CDM_H

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

class LLVM_LIBRARY_VISIBILITY CDMToolChain : public ToolChain {

  class CDMToolChainInstallationDetector {

    std::optional<std::string> CocasPath; // Cocas executable path
    std::optional<std::string> LibPath;
    std::optional<std::string> IncludePath;

  public:
    CDMToolChainInstallationDetector(const Driver &D);

    std::optional<std::string> getCocasPath() const { return CocasPath; }
    std::optional<std::string> getLibPath() const { return LibPath; }
    std::optional<std::string> getIncludePath() const { return IncludePath; }
  };

  const CDMToolChainInstallationDetector CDMInstallation;
  // TODO: Add object files from standard lib
  const std::vector<const char *> stdLibObjs = {};

public:
  CDMToolChain(const Driver &D, const llvm::Triple &Triple,
               const llvm::opt::ArgList &Args)
      : ToolChain(D, Triple, Args), CDMInstallation(D) {};

  // CdM hasn't integrated assembler, it must use cocas
  bool useIntegratedAs() const override { return false; }
  // CdM hasn't integrated obj emitter
  bool useIntegratedBackend() const override { return false; }
  bool IsIntegratedBackendDefault() const override { return false; }
  bool IsIntegratedBackendSupported() const override { return false; }
  bool IsNonIntegratedBackendSupported() const override { return true; }

  bool isPICDefault() const override { return false; }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }

  // We don't have PIE or PIC, so ignore all PIC-related flags and always
  // generate non-PIC code
  bool isPICDefaultForced() const override { return true; }

  std::string getInputFilename(const InputInfo &Input) const override;

  llvm::opt::DerivedArgList *
  TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
                Action::OffloadKind DeviceOffloadKind) const override;

  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;

  std::string getCompilerRTPath() const override;

  bool SupportsProfiling() const override { return false; }

  CDMToolChainInstallationDetector getCDMInstallation() const {
    return CDMInstallation;
  }
  const std::vector<const char *> &getStdLibObjs() const { return stdLibObjs; }

protected:
  Tool *buildAssembler() const override; // cocas
  Tool *buildLinker() const override;    // cocas
};

} // end namespace toolchains

namespace tools {
namespace CDM {

class LLVM_LIBRARY_VISIBILITY Cocas final : public Tool {

  std::string cocasPath;

public:
  Cocas(const toolchains::CDMToolChain &TC);

  bool hasIntegratedCPP() const override { return false; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &Args,
                    const char *LinkingOutput) const override;

  const toolchains::CDMToolChain &getCDMToolChain() const {
    return static_cast<const toolchains::CDMToolChain &>(getToolChain());
  }
};

} // end namespace CDM
} // end namespace tools
} // end namespace driver
} // end namespace clang

#endif
