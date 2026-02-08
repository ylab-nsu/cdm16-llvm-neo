#ifndef LLVM_LIB_TARGET_CDM_CDMTARGETMACHINE_H
#define LLVM_LIB_TARGET_CDM_CDMTARGETMACHINE_H

#include "CDMSubtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class CDMTargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  const DataLayout DataLayout;
  CDMSubtarget DefaultSubtarget;

public:
  CDMTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM,
                   std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                   bool JIT);
  ~CDMTargetMachine() override;

  virtual const class DataLayout *getDataLayout() const { return &DataLayout; }
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  const CDMSubtarget *getSubtargetImpl(const Function &F) const override {
    return &DefaultSubtarget;
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};

} // namespace llvm
#endif
