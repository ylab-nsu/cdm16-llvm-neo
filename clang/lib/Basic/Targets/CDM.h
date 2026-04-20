#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_CDM_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_CDM_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

namespace clang {
namespace targets {
class LLVM_LIBRARY_VISIBILITY CDMTargetInfo : public TargetInfo {
public:
  CDMTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    TLSSupported = false;
    PointerWidth = 16;
    PointerAlign = 16;
    IntWidth = 16;
    IntAlign = 16;
    LongWidth = 32;
    LongAlign = 16;
    LongLongWidth = 64;
    LongLongAlign = 16;
    SuitableAlign = 8;
    DefaultAlignForAttributeAligned = 8;
    HalfWidth = 16;
    HalfAlign = 16;
    FloatWidth = 32;
    FloatAlign = 16;
    DoubleWidth = 64;
    DoubleAlign = 16;
    DoubleFormat = &llvm::APFloat::IEEEdouble();
    LongDoubleWidth = 128;
    LongDoubleAlign = 16;
    LongDoubleFormat = &llvm::APFloat::IEEEquad();
    SizeType = UnsignedInt;
    PtrDiffType = SignedInt;
    IntPtrType = SignedInt;
    Int16Type = SignedInt;
    Int64Type = SignedLongLong;
    IntMaxType = SignedLongLong;
    WCharType = SignedLong;
    WIntType = SignedLong;
    Char16Type = UnsignedInt;
    Char32Type = UnsignedLong;
    SigAtomicType = SignedChar;
    resetDataLayout("e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-"
                    "f64:16-f128:16-n16-S16");
  }

  bool allowsLargerPreferedTypeAlignment() const override { return false; }

  llvm::SmallVector<Builtin::InfosShard> getTargetBuiltins() const override;

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  std::string_view getClobbers() const override { return ""; }

  ArrayRef<const char *> getGCCRegNames() const override {
    static const char *const GCCRegNames[] = {"r0", "r1", "r2", "r3",
                                              "r4", "r5", "r6", "r7"};
    return llvm::ArrayRef(GCCRegNames);
  }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    static const TargetInfo::GCCRegAlias GCCRegAliases[] = {
        {{"fp"}, "r7"},
    };
    return GCCRegAliases;
  }

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    // No target-specific asm constraints.
    return false;
  }

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {
    switch (CC) {
    default:
      return CCCR_Error;
    case CC_C:
    case CC_CdmIsr:
      return CCCR_OK;
    }
  }
};
} // namespace targets
} // namespace clang

#endif
