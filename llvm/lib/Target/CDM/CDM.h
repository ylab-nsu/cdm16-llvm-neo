#ifndef LLVM_LIB_TARGET_CDM_CDM_H
#define LLVM_LIB_TARGET_CDM_CDM_H

#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"

namespace llvm {

class CDMTargetMachine;
class FunctionPass;
class PassRegistry;

FunctionPass *createCDMISelDag(CDMTargetMachine &TM, CodeGenOptLevel OptLevel);
FunctionPass *createCDMFrameAnalyzerPass();

void initializeCDMAsmPrinterPass(PassRegistry &);
void initializeCDMDAGToDAGISelLegacyPass(PassRegistry &);

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
