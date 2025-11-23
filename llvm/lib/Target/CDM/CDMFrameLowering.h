//
// Created by ilya on 28.10.23.
//

#ifndef LLVM_CDMFRAMELOWERING_H
#define LLVM_CDMFRAMELOWERING_H

#include "CDM.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class CDMSubtarget;
class CDMFrameLowering : public TargetFrameLowering {
public:
  explicit CDMFrameLowering(const CDMSubtarget &Sti)
      : TargetFrameLowering(StackGrowsDown, Align(2), 0, Align(2)), STI(Sti) {}
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  // Eliminate ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo instructions
  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const override;

protected:
  bool hasFPImpl(const MachineFunction &MF) const override;

private:
  const CDMSubtarget &STI;
  static void ensureStackFrameAddressable(const MachineFunction &MF);
};

} // namespace llvm

#endif // LLVM_CDMFRAMELOWERING_H
