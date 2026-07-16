#include "Cocas.h"
#include "CDM.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/InputInfo.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include "clang/Driver/Types.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/TargetParser/Triple.h"
#include <cstdlib>
#include <cstring>
#include <optional>

using namespace llvm::opt;
using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace clang::driver::tools;

CDM::Cocas::Cocas(const CocasToolChain &TC) : Tool("CDM::Cocas", "cocas", TC) {
  if (!TC.getCocasPath()) {
    llvm_unreachable("Cannot create cocas object without path to cocas");
  }
}

void CDM::Cocas::ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &Args,
                              const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  // Claim all -mmem-model= options when linking
  // to behave like the cocas-less target
  if (JA.getKind() == Action::LinkJobClass) {
    Args.claimAllArgs(options::OPT_mmem_model_EQ);
  }

  // If job kind is Assemble, only assemble, don't link
  if (JA.getKind() == Action::AssembleJobClass) {
    CmdArgs.push_back("-c");
  }
  // If job kind is link and incremental linking requested, "Merge" objects
  else if (JA.getKind() == Action::LinkJobClass &&
           Args.hasArg(options::OPT_r)) {
    CmdArgs.push_back("-m");
  }

  // If -g flag provided, add --debug flag
  if (Args.hasArg(options::OPT_g_Flag)) {
    CmdArgs.push_back("--debug");

    // If linking, specify file for exporting debug info
    if (JA.getKind() == Action::LinkJobClass) {
      // If cocas-debug-output specified, use this file
      if (Arg *A = Args.getLastArg(options::OPT_cocas_debug_output)) {
        CmdArgs.push_back(A->getValue());
      }
      // Fallback to default file for debug info
      else {
        CmdArgs.push_back("out.dbg.json");
      }
    }
  }

  // Target settings
  CmdArgs.push_back("-t");
  CmdArgs.push_back(
      Args.MakeArgString(Args.getLastArgValue(options::OPT_march_EQ, "cdm16")));

  // Add output file
  CmdArgs.push_back("-o");
  CmdArgs.push_back(Args.MakeArgString(Output.getFilename()));

  // Add all input files
  for (const auto &II : Inputs) {
    if (II.isFilename()) {
      CmdArgs.push_back(Args.MakeArgString(II.getFilename()));
    }
  }

  if (JA.getKind() == Action::LinkJobClass) {
    if (!Args.hasArg(options::OPT_nostartfiles, options::OPT_nostdlib,
                     options::OPT_r)) {
      CmdArgs.push_back(
          Args.MakeArgString(getCocasToolChain().GetFilePath("crt0.o")));
    }
    if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs,
                     options::OPT_r)) {
      // Add object files from standard lib
      for (const char *obj : getCocasToolChain().getStdLibObjs()) {
        CmdArgs.push_back(
            Args.MakeArgString(getCocasToolChain().GetFilePath(obj)));
      }
    }
    if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs,
                     options::OPT_r)) {
      // Add builtins
      for (const char *obj : getCocasToolChain().getBuiltinNames()) {
        CmdArgs.push_back(getCocasToolChain().getCompilerRTArgString(
            Args, obj, ToolChain::FT_Static));
      }
    }

    // User search paths
    std::vector<std::string> libSearchDirs =
        Args.getAllArgValues(options::OPT_L);

    // Default search paths
    for (auto LibPath : getCocasToolChain().getFilePaths()) {
      libSearchDirs.push_back(LibPath);
    }

    std::vector<std::string> libsToLink = Args.getAllArgValues(options::OPT_l);

    // Link files, provided with -l option
    SmallString<128> P;
    for (const std::string &lib : libsToLink) {
      bool found = false;
      for (const std::string &dir : libSearchDirs) {
        P = dir;
        if (lib[0] == ':') {
          llvm::sys::path::append(P, lib.c_str() + 1);
        } else {
          llvm::sys::path::append(P, lib + ".lib");
        }

        if (llvm::sys::fs::exists(Twine(P))) {
          CmdArgs.push_back(Args.MakeArgString(P));
          found = true;
          break;
        }
      }

      if (!found) {
        C.getDriver().Diag(clang::diag::err_drv_no_such_file) << lib;
      }
    }
  }

  C.addCommand(std::make_unique<Command>(
      JA, *this, ResponseFileSupport::None(),
      Args.MakeArgString(*getCocasToolChain().getCocasPath()), CmdArgs, Inputs,
      Output));
}

CocasToolChain::CocasToolChain(const Driver &D, const llvm::Triple &Triple,
                               const llvm::opt::ArgList &Args)
    : ToolChain(D, Triple, Args) {
  char *CocasEnv = std::getenv("COCAS");

  // If COCAS env defined, get it as path to cocas
  if (CocasEnv != NULL) {
    CocasPath = CocasEnv;
  }
  // Try to search the toolchain binary directory
  else if (llvm::ErrorOr<std::string> P =
               llvm::sys::findProgramByName("cocas", {getDriver().Dir})) {
    CocasPath = *P;
  }
  // Try to search cocas in PATH
  else if (llvm::ErrorOr<std::string> P =
               llvm::sys::findProgramByName("cocas")) {
    CocasPath = *P;
  }
  // Try to search cocas in current directory
  else if (llvm::ErrorOr<std::string> P =
               llvm::sys::findProgramByName("cocas", {"."})) {
    CocasPath = *P;
  }

  // Since cdm and cdm-cocas share same headers, strip environment
  const llvm::Triple T = llvm::Triple(getTriple().getArchName());
  SmallString<128> P(D.Dir);
  llvm::sys::path::append(P, "..", "include", T.str());

  if (llvm::sys::fs::is_directory(P)) {
    IncludePath = P.str();
  }
}

DerivedArgList *
CocasToolChain::TranslateArgs(const llvm::opt::DerivedArgList &Args,
                              StringRef BoundArch,
                              Action::OffloadKind DeviceOffloadKind) const {
  DerivedArgList *DAL = new DerivedArgList(Args.getBaseArgs());

  // Check that the memory model is valid and supported
  bool MemModelValid =
      checkMemoryModel(getDriver(), Args, {MemoryModel::VonNeumann});

  for (Arg *A : Args) {
    if (A->getOption().matches(options::OPT_DebugInfo_Group)) {
      // Remove all debug-related options and warn user, that they're ignored
      // TODO: Remove this when C debug info emitting to objects in cocas got
      // done
      getDriver().Diag(clang::diag::warn_drv_unsupported_option_for_target)
          << A->getAsString(Args) << getTripleString();

      // Claim arg to avoid getting unused argument warn
      A->claim();
    } else if (!MemModelValid &&
               A->getOption().matches(options::OPT_mmem_model_EQ)) {
      // Remove all -mmem-model= arguments if the specified memory model is
      // unsupported.
      A->claim();
    } else {
      DAL->append(A);
    }
  }

  return DAL;
}

void CocasToolChain::AddClangSystemIncludeArgs(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args) const {

  if (DriverArgs.hasArg(options::OPT_nostdinc) ||
      DriverArgs.hasArg(options::OPT_nostdlibinc)) {
    return;
  }
  if (getIncludePath()) {
    addSystemInclude(DriverArgs, CC1Args, *getIncludePath());
  }
}

void CocasToolChain::addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
                                           llvm::opt::ArgStringList &CC1Args,
                                           Action::OffloadKind Ofk) const {
  MemoryModel MemModel = getMemoryModel(getDriver(), DriverArgs);
  switch (MemModel) {
  case MemoryModel::VonNeumann:
    CC1Args.push_back("-D__VON_NEUMANN__");
    break;
  case MemoryModel::Harvard:
    CC1Args.push_back("-D__HARVARD__");
    break;
  }
}

Tool *CocasToolChain::buildAssembler() const {
  if (!getCocasPath()) {
    getDriver().Diag(clang::diag::err_drv_no_cocas_assembler);
    return nullptr;
  }
  return new CDM::Cocas(*this);
}

Tool *CocasToolChain::buildLinker() const {
  if (!getCocasPath()) {
    getDriver().Diag(clang::diag::err_drv_no_cocas_linker);
    return nullptr;
  }
  return new CDM::Cocas(*this);
}
