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
#include <cstdlib>
#include <cstring>
#include <optional>

using namespace llvm::opt;
using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace clang::driver::tools;

CDM::Cocas::Cocas(const CDMToolChain &TC) : Tool("CDM::Cocas", "cocas", TC) {
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

  // If job kind is Assemble, only assemble, don't link
  if (JA.getKind() == Action::AssembleJobClass) {
    CmdArgs.push_back("-c");
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

  // Target cdm16
  CmdArgs.push_back("-t");
  CmdArgs.push_back("cdm16");

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
    // Add object files from standard lib
    for (const char *obj : getCDMToolChain().getStdLibObjs()) {
      CmdArgs.push_back(Args.MakeArgString(getCDMToolChain().GetFilePath(obj)));
    }
    // Add builtins
    for (const char *obj : getCDMToolChain().getBuiltinNames()) {
      CmdArgs.push_back(getCDMToolChain().getCompilerRTArgString(Args, obj, ToolChain::FT_Object));
    }

    std::vector<std::string> libSearchDirs =
        Args.getAllArgValues(options::OPT_L);
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
      Args.MakeArgString(*getCDMToolChain().getCocasPath()), CmdArgs, Inputs,
      Output));
}

CDMToolChain::CDMToolChain(const Driver &D, const llvm::Triple &Triple,
                           const llvm::opt::ArgList &Args)
    : ToolChain(D, Triple, Args) {
  char *CocasEnv = std::getenv("COCAS");

  // If COCAS env defined, get it as path to cocas
  if (CocasEnv != NULL) {
    CocasPath = CocasEnv;
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

  // TODO: Set Include Path and push any Lib paths to ToolChain::LibraryPaths
}

DerivedArgList *
CDMToolChain::TranslateArgs(const llvm::opt::DerivedArgList &Args,
                            StringRef BoundArch,
                            Action::OffloadKind DeviceOffloadKind) const {
  DerivedArgList *DAL = new DerivedArgList(Args.getBaseArgs());

  // TODO: Remove this when C debug info emitting to objects in cocas got done

  // Remove all debug-related option and warn user about their ignorance
  for (Arg *A : Args) {
    if (A->getOption().matches(options::OPT_DebugInfo_Group)) {
      getDriver().Diag(clang::diag::warn_drv_unsupported_option_for_target)
          << A->getAsString(Args) << getTripleString();

      // Claim arg to avoid getting unused argument warn
      A->claim();
    } else {
      DAL->append(A);
    }
  }

  return DAL;
}

void CDMToolChain::AddClangSystemIncludeArgs(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args) const {
  if (getIncludePath()) {
    CC1Args.append(
        {"-internal-isystem", DriverArgs.MakeArgString(*getIncludePath())});
  }
}

Tool *CDMToolChain::buildAssembler() const {
  if (!getCocasPath()) {
    getDriver().Diag(clang::diag::err_drv_no_cocas_assembler);
    return nullptr;
  }
  return new CDM::Cocas(*this);
}

Tool *CDMToolChain::buildLinker() const {
  if (!getCocasPath()) {
    getDriver().Diag(clang::diag::err_drv_no_cocas_linker);
    return nullptr;
  }
  return new CDM::Cocas(*this);
}
