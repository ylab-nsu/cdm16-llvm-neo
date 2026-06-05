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

void CDMToolChain::addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
                                         llvm::opt::ArgStringList &CC1Args,
                                         Action::OffloadKind Ofk) const {
  Generic_ELF::addClangTargetOptions(DriverArgs, CC1Args, Ofk);

  std::optional<CDMToolChain::MemoryModel> MemModel =
      CDMToolChain::getMemoryModel(getDriver(), DriverArgs);
  if (!MemModel) {
    return;
  }
  switch (MemModel.value()) {
  case MemoryModel::VonNeumann:
    CC1Args.push_back("-D__VON_NEUMANN__");
    break;
  case MemoryModel::Harvard:
    CC1Args.push_back("-D__HARVARD__");
    break;
  }
}

std::optional<CDMToolChain::MemoryModel>
CDMToolChain::getMemoryModel(const Driver &D, const llvm::opt::ArgList &Args) {
  Arg *A = Args.getLastArg(options::OPT_mmem_model_EQ);
  if (!A) {
    return MemoryModel::VonNeumann;
  }

  StringRef Val = A->getValue();
  std::optional<MemoryModel> Model =
      llvm::StringSwitch<std::optional<MemoryModel>>(Val)
          .Case("vonNeumann", MemoryModel::VonNeumann)
          .Case("harvard", MemoryModel::Harvard)
          .Case("vn", MemoryModel::VonNeumann)
          .Case("hv", MemoryModel::Harvard)
          .Default(std::nullopt);
  if (!Model) {
    D.Diag(diag::err_drv_unsupported_option_argument)
        << A->getSpelling() << Val;
  }
  return Model;
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

  std::optional<CDMToolChain::MemoryModel> MemModel =
      CDMToolChain::getMemoryModel(D, Args);
  if (!MemModel) {
    return;
  }
  if (!Args.hasArg(options::OPT_r, options::OPT_T)) {
    for (const char *Script :
         getCDMToolChain().getLinkerScripts(MemModel.value())) {
      CmdArgs.push_back("-T");
      CmdArgs.push_back(
          Args.MakeArgString(getCDMToolChain().GetFilePath(Script)));
    }
  }
  Args.addAllArgs(CmdArgs, {options::OPT_T, options::OPT_t});

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nostartfiles,
                   options::OPT_r)) {
    for (const char *Obj : getCDMToolChain().getStartFiles(MemModel.value())) {
      CmdArgs.push_back(Args.MakeArgString(getCDMToolChain().GetFilePath(Obj)));
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs,
                   options::OPT_r)) {
    for (const char *Lib : getCDMToolChain().getStdLibs()) {
      CmdArgs.push_back("-l");
      CmdArgs.push_back(Lib);
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
