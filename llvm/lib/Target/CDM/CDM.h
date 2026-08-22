#ifndef LLVM_LIB_TARGET_CDM_CDM_H
#define LLVM_LIB_TARGET_CDM_CDM_H

#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Pass.h"

namespace llvm {

FunctionPass *createCDMFrameAnalyzerPass();

enum AddressSpace {
  DataMemory,
  ProgramMemory,
  NumAddrSpaces,
};

template <typename T> bool isProgramMemoryAddress(T *V) {
  auto *PT = cast<PointerType>(V->getType());
  assert(PT != nullptr && "unexpected MemSDNode");
  return PT->getAddressSpace() == ProgramMemory;
}

template <typename T> AddressSpace getAddressSpace(T *V) {
  auto *PT = cast<PointerType>(V->getType());
  assert(PT != nullptr && "unexpected MemSDNode");
  unsigned AS = PT->getAddressSpace();
  if (AS < NumAddrSpaces) {
    return static_cast<AddressSpace>(AS);
  }
  return NumAddrSpaces;
}

inline bool isProgramMemoryAccess(MemSDNode const *N) {
  auto *V = N->getMemOperand()->getValue();
  if (V != nullptr && isProgramMemoryAddress(V)) {
    return true;
  }
  return false;
}

} // namespace llvm

#endif
