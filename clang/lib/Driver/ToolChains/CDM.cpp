#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/FileSystem.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/InputInfo.h"
#include "clang/Driver/Types.h"
#include "CDM.h"
#include <cstdlib>
#include <cstring>

using namespace llvm::opt;
using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace clang::driver::tools;

void CDM::Cocas::ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &Args,
                    const char *LinkingOutput) const {
  const auto &TC =
      static_cast<const toolchains::CDMToolChain &>(getToolChain());
  ArgStringList CmdArgs;

  if (!TC.getCDMInstallation().isValid()) {
    return;
  }


  // If job kind is Assemble, only assemble, don't link
  if (JA.getKind() == Action::AssembleJobClass){
    CmdArgs.push_back("-c");
  }
  
  // If -g flag provided, add --debug flag
  if (Args.hasArg(options::OPT_g_Flag)){
    CmdArgs.push_back("--debug");

    // If linking, specify file for exporting debug info
    if (JA.getKind() == Action::LinkJobClass){
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
  const char *FinalOutput = C.getArgs().getLastArg(options::OPT_o)->getValue();
  // If output is final output, don't change filename
  if (std::strcmp(FinalOutput, Output.getFilename()) == 0) {
    CmdArgs.push_back(Args.MakeArgString(Output.getFilename()));
  }
  else {
    CmdArgs.push_back(Args.MakeArgString(TC.getInputFilename(Output)));
  }

  // Add all input files
  for (const auto &II : Inputs){
    CmdArgs.push_back(Args.MakeArgString(TC.getInputFilename(II)));
  }

  if (JA.getKind() == Action::LinkJobClass) {
    for (const std::string &obj : TC.getStdLibObjs()) {
      CmdArgs.push_back(Args.MakeArgString(TC.GetFilePath(obj)));
    }
  }

  Args.addAllArgs(CmdArgs, {options::OPT_L});

  const char *Exec = Args.MakeArgString(TC.getCDMInstallation().getCocasPath());

  C.addCommand(std::make_unique<Command>(
      JA, *this,
      ResponseFileSupport::None(),
      Exec, CmdArgs, Inputs, Output));
}

CDMToolChain::CDMToolChainInstallationDetector::CDMToolChainInstallationDetector(const Driver &D) : IsValid(true) {
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
  else {
    IsValid = false;
  }
	
  // TODO: Set Lib and Include Path

  if (!IsValid) {
    D.Diag(clang::diag::err_drv_no_cocas_installation);
  }
}

std::string CDMToolChain::getCompilerRTPath() const {
  return CDMInstallation.getLibPath();
}

DerivedArgList *
CDMToolChain::TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
              Action::OffloadKind DeviceOffloadKind) const {
  DerivedArgList *DAL = new DerivedArgList(Args.getBaseArgs());

  // TODO: Remove this when C debug info emitting to objects in cocas got done

  // Remove -g option and warn user about its ignorance
  for (Arg *A : Args) {
    if (A->getOption().getID() == options::OPT_g_Flag) {
      getDriver().Diag(clang::diag::warn_drv_unsupported_option_for_target)
	      << A->getAsString(Args) << getTripleString();

      // Claim arg to avoid getting unused argument warn
      A->claim();
    }
    else {
      DAL->append(A);
    }
  }

  return DAL;
}

std::string CDMToolChain::getInputFilename(const InputInfo &Input) const {
	std::string filename = Input.getFilename();

	// We must use .obj for object files, because cocas need this suffix
	// to recognize filetype
	if (Input.getType() == types::TY_Object){
	  return filename.substr(0, filename.find_last_of('.')) + ".obj";
	}

	return filename;
}

void CDMToolChain::AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                          llvm::opt::ArgStringList &CC1Args) const {
  if (!CDMInstallation.getIncludePath().empty() && CDMInstallation.isValid()) {
    CC1Args.append(
        {"-internal-isystem",
         DriverArgs.MakeArgString(CDMInstallation.getIncludePath())});
  }
}

Tool *CDMToolChain::buildAssembler() const {
  return new CDM::Cocas(*this);
}

Tool *CDMToolChain::buildLinker() const {
  return new CDM::Cocas(*this);
}

