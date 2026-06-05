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
  enum class MemoryModel {
    VonNeumann,
    Harvard,
  };

  CDMToolChain(const Driver &D, const llvm::Triple &Triple,
               const llvm::opt::ArgList &Args)
      : Generic_ELF(D, Triple, Args) {}

  const char *getDefaultLinker() const override { return "ld.lld"; }

  bool HasNativeLLVMSupport() const override { return true; }

  bool isPICDefault() const override { return false; }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }

  // We don't have PIE or PIC, so we must ignore all PIC-related flags and
  // always generate non-PIC code
  bool isPICDefaultForced() const override { return true; }

  bool SupportsProfiling() const override { return false; }

  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;

  void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
                             llvm::opt::ArgStringList &CC1Args,
                             Action::OffloadKind) const override;

  const std::vector<const char *> &getStdLibs() const { return StdLibs; }
  const std::vector<const char *> &getBuiltinNames() const {
    return BuiltinNames;
  }
  const std::vector<const char *> &getStartFiles(MemoryModel MemModel) const {
    switch (MemModel) {
    case MemoryModel::VonNeumann:
      return StartFilesVonNeumann;
    case MemoryModel::Harvard:
      return StartFilesHarvard;
    }
  }
  const std::vector<const char *> &
  getLinkerScripts(MemoryModel MemModel) const {
    switch (MemModel) {
    case MemoryModel::VonNeumann:
      return LinkerScriptsVonNeumann;
    case MemoryModel::Harvard:
      return LinkerScriptsHarvard;
    }
  }

  static std::optional<MemoryModel>
  getMemoryModel(const Driver &D, const llvm::opt::ArgList &Args);

protected:
  Tool *buildLinker() const override;

private:
  const std::vector<const char *> StdLibs = {"c"};
  const std::vector<const char *> BuiltinNames = {"cdm-builtins"};

  const std::vector<const char *> StartFilesVonNeumann = {"crt0-vonNeumann.o"};
  const std::vector<const char *> StartFilesHarvard = {"crt0-harvard.o"};
  const std::vector<const char *> LinkerScriptsVonNeumann = {
      "ldscripts/cdm-vonNeumann.ld"};
  const std::vector<const char *> LinkerScriptsHarvard = {
      "ldscripts/cdm-harvard.ld"};
};

} // end namespace toolchains
namespace tools {
namespace CDM {

class LLVM_LIBRARY_VISIBILITY LldLinker final : public Tool {
public:
  LldLinker(const ToolChain &TC) : Tool("CDM::Linker", "ld.lld", TC) {}

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
