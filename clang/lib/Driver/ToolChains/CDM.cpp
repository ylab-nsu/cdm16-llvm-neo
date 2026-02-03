#include "CDM.h"
#include "clang/Config/config.h"
#include "clang/Driver/CommonArgs.h"

using namespace llvm::opt;
using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace clang::driver::tools;

Tool *CDMToolChain::buildLinker() const {
  return new tools::CDM::Linker(*this);
}

void CDM::Linker::ConstructJob(Compilation &C, const JobAction &JA,
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

  if (!Args.hasArg(options::OPT_r) && !Args.hasArg(options::OPT_T)) {
    CmdArgs.push_back("--oformat=binary");
  }

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  Args.addAllArgs(CmdArgs, {options::OPT_L, options::OPT_u});

  if (D.isUsingLTO()) {
    addLTOOptions(getToolChain(), Args, CmdArgs, Output, Inputs,
                  D.getLTOMode() == LTOK_Thin);
  }

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs, JA);

  Args.addAllArgs(CmdArgs, {options::OPT_T, options::OPT_t});

  const char *Exec = Args.MakeArgString(getToolChain().GetLinkerPath());
  C.addCommand(std::make_unique<Command>(JA, *this,
                                         ResponseFileSupport::AtFileCurCP(),
                                         Exec, CmdArgs, Inputs, Output));
}
