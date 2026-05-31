#include "CDM.h"
#include "clang/Config/config.h"
#include "clang/Driver/CommonArgs.h"

using namespace llvm::opt;
using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace clang::driver::tools;

Tool *CDMToolChain::buildLinker() const {
  return new tools::CDM::LldLinker(*this);
}

void CDMToolChain::AddClangSystemIncludeArgs(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args) const {

  if (DriverArgs.hasArg(options::OPT_nostdinc) ||
      DriverArgs.hasArg(options::OPT_nostdlibinc)) {
    return;
  }
  if (getStdlibIncludePath()) {
    addSystemInclude(DriverArgs, CC1Args, *getStdlibIncludePath());
  }
}

void CDM::LldLinker::ConstructJob(Compilation &C, const JobAction &JA,
                                  const InputInfo &Output,
                                  const InputInfoList &Inputs,
                                  const llvm::opt::ArgList &Args,
                                  const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();

  ArgStringList CmdArgs;

  // Silence warning for "clang -g foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_g_Group);
  // and "clang -emit-llvm foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_emit_llvm);
  // and for "clang -w foo.o -o foo". Other warning options are already
  // handled somewhere else.
  Args.ClaimAllArgs(options::OPT_w);

  if (Args.hasArg(options::OPT_s)) {
    CmdArgs.push_back("-s");
  }

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  Args.addAllArgs(CmdArgs, {options::OPT_L, options::OPT_u});
  getToolChain().AddFilePathLibArgs(Args, CmdArgs);

  if (D.isUsingLTO()) {
    addLTOOptions(getToolChain(), Args, CmdArgs, Output, Inputs,
                  D.getLTOMode() == LTOK_Thin);
  }

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs, JA);

  if (!Args.hasArg(options::OPT_r, options::OPT_T)) {
    CmdArgs.push_back("-T");
    CmdArgs.push_back(
        Args.MakeArgString(getCDMToolChain().GetFilePath("ldscripts/cdm.ld")));
  }
  Args.addAllArgs(CmdArgs, {options::OPT_T, options::OPT_t});

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nostartfiles,
                   options::OPT_r)) {
    for (const char *obj : getCDMToolChain().getStartFiles()) {
      CmdArgs.push_back(Args.MakeArgString(getCDMToolChain().GetFilePath(obj)));
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs,
                   options::OPT_r)) {
    for (const char *lib : getCDMToolChain().getStdLibs()) {
      CmdArgs.push_back("-l");
      CmdArgs.push_back(lib);
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs,
                   options::OPT_r)) {
    for (const char *obj : getCDMToolChain().getBuiltinNames()) {
      CmdArgs.push_back(getCDMToolChain().getCompilerRTArgString(
          Args, obj, ToolChain::FT_Static));
    }
  }

  const char *Exec = Args.MakeArgString(getToolChain().GetLinkerPath());
  C.addCommand(std::make_unique<Command>(JA, *this,
                                         ResponseFileSupport::AtFileCurCP(),
                                         Exec, CmdArgs, Inputs, Output));
}
