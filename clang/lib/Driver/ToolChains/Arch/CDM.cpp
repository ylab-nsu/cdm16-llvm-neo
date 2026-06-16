#include "CDM.h"
#include "llvm/TargetParser/CDMTargetParser.h"

#include "clang/Driver/Driver.h"
#include "llvm/Option/ArgList.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang;
using namespace llvm::opt;

void cdm::getCDMTargetFeatures(const Driver &D, const llvm::Triple &Triple,
                               const ArgList &Args,
                               std::vector<StringRef> &Features) {

  const Arg *MArch = Args.getLastArg(options::OPT_march_EQ);
  if (!MArch) {
    return;
  }
  StringRef ArchName = MArch->getValue();
  if (!llvm::CDM::getArchFeatures(ArchName, Features)) {
    D.Diag(diag::err_drv_invalid_arch_name) << ArchName;
  }
}
