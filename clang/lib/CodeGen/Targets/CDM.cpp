#include "ABIInfoImpl.h"
#include "TargetInfo.h"
#include "clang/Basic/DiagnosticFrontend.h"

using namespace clang;
using namespace clang::CodeGen;

namespace {

class CDMTargetCodeGenInfo : public TargetCodeGenInfo {
public:
  CDMTargetCodeGenInfo(CodeGenTypes &CGT)
      : TargetCodeGenInfo(std::make_unique<DefaultABIInfo>(CGT)) {}
  LangAS getGlobalVarAddressSpace(CodeGenModule &CGM,
                                  const VarDecl *D) const override;
  void setTargetAttributes(const Decl *D, llvm::GlobalValue *GV,
                           CodeGen::CodeGenModule &M) const override;
};

} // namespace

LangAS CDMTargetCodeGenInfo::getGlobalVarAddressSpace(CodeGenModule &CGM,
                                                      const VarDecl *D) const {
  if (D) {
    LangAS AS = D->getType().getAddressSpace();
    if (isTargetAddressSpace(AS) && toTargetAddressSpace(AS) == 1 &&
        !D->getType().isConstQualified()) {
      CGM.getDiags().Report(D->getLocation(),
                            diag::err_verify_nonconst_addrspace)
          << "__progmem";
    }
  }
  return TargetCodeGenInfo::getGlobalVarAddressSpace(CGM, D);
}

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
