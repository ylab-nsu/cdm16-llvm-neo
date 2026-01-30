//
// Created by ilya on 21.10.23.
//

#ifndef LLVM_CDMTARGETOBJECTFILE_H
#define LLVM_CDMTARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
namespace llvm {

class CDMTargetObjectFile : public TargetLoweringObjectFileELF {
public:
  unsigned getTextSectionAlignment() const override { return 2; }

  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};

} // namespace llvm

#endif // LLVM_CDMTARGETOBJECTFILE_H
