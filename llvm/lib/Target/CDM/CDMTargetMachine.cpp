#include "CDMTargetMachine.h"
#include "CDMFunctionInfo.h"
#include "CDMIselDAGToDAG.h"
#include "CDMTargetObjectFile.h"
#include "TargetInfo/CDMTargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include <optional>
using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCDMTarget() {
  // Register the target.
  RegisterTargetMachine<CDMTargetMachine> X(getTheCDMTarget());

  //  PassRegistry &PR = *PassRegistry::getPassRegistry();
  //  initializeSparcDAGToDAGISelPass(PR);
}

static std::string computeDataLayout() {
  // XXX Build the triple from the arguments.
  // This is hard-coded for now for this example target.
  return "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-"
         "m:C-n16";
}

CDMTargetMachine::CDMTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, computeDataLayout(), TT, CPU, FS, Options,
                               Reloc::Static, CodeModel::Small, OL),
      TLOF(std::make_unique<CDMTargetObjectFile>()),
      DataLayout(computeDataLayout()), DefaultSubtarget(TT, CPU, FS, *this) {
  initAsmInfo();
  //  Options.EmitAddrsig = false;
}
CDMTargetMachine::~CDMTargetMachine() = default;

MachineFunctionInfo *CDMTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return CDMFunctionInfo::create<CDMFunctionInfo>(Allocator, F, STI);
}

namespace {
class CDMPassConfig : public TargetPassConfig {
public:
  CDMPassConfig(CDMTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {
    EnableTailMerge = false;
  }

  bool addInstSelector() override;
};

} // namespace

TargetPassConfig *CDMTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new CDMPassConfig(*this, PM);
}

bool CDMPassConfig::addInstSelector() {
  addPass(createCDMISelDagLegacy(getTM<CDMTargetMachine>(), getOptLevel()));
  return false;
}
