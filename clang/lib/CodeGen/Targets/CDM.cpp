#include "ABIInfoImpl.h"
#include "TargetInfo.h"

using namespace clang;
using namespace clang::CodeGen;

namespace {

class CDMTargetCodeGenInfo : public TargetCodeGenInfo {
public:
  CDMTargetCodeGenInfo(CodeGenTypes &CGT)
      : TargetCodeGenInfo(std::make_unique<DefaultABIInfo>(CGT)) {}
  void setTargetAttributes(const Decl *D, llvm::GlobalValue *GV,
                           CodeGen::CodeGenModule &M) const override;
};

} // namespace

void CDMTargetCodeGenInfo::setTargetAttributes(
    const Decl *D, llvm::GlobalValue *GV, CodeGen::CodeGenModule &M) const {
  if (const auto *FD = dyn_cast_or_null<FunctionDecl>(D)) {
    if (FD->getAttr<CDMInterruptAttr>()) {
      llvm::Function *F = cast<llvm::Function>(GV);
      F->setCallingConv(llvm::CallingConv::CDM_INTR);
      F->addFnAttr(llvm::Attribute::NoInline);
    }
  }
}

std::unique_ptr<TargetCodeGenInfo>
CodeGen::createCDMTargetCodeGenInfo(CodeGenModule &CGM) {
  return std::make_unique<CDMTargetCodeGenInfo>(CGM.getTypes());
}
