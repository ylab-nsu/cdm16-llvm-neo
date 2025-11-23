//
// Created by ilya on 28.10.23.
//

#include "CDMFrameLowering.h"

#include "CDMInstrInfo.h"
#include "CDMSubtarget.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/ADT/Twine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

namespace llvm {

void CDMFrameLowering::ensureStackFrameAddressable(const MachineFunction &MF) {
  static int DK_LargeStackFrameSize = // NOLINT
      llvm::getNextAvailablePluginDiagnosticKind();

  struct DiagnosticInfoLargeStackSize : public DiagnosticInfo {
    StringRef Msg;
    DiagnosticInfoLargeStackSize(StringRef Msg)
        : DiagnosticInfo{DK_LargeStackFrameSize, DS_Error}, Msg{Msg} {}
    void print(DiagnosticPrinter &DP) const override { DP << Msg; }
    static bool classof(const DiagnosticInfo *DI) {
      return DI->getKind() == DK_LargeStackFrameSize;
    }
  };

  uint64_t FrameSize = MF.getFrameInfo().getStackSize();
  // More than 2^15 will be impossible to address in the current scheme.
  uint64_t Limit = std::numeric_limits<int16_t>::max();

  if (FrameSize > Limit) {
    const Twine &ErrMsg = "stack frame size (" + Twine(FrameSize) +
                          ") in function " + "\'" + MF.getName() + "\'" +
                          " exceeds limit (" + Twine(Limit) +
                          "), consider writing better code please.";

    MF.getFunction().getContext().diagnose(
        DiagnosticInfoLargeStackSize(ErrMsg.str()));
  }
}

//- Must have, hasFP() is pure virtual of parent
// hasFP - Return true if the specified function should have a dedicated frame
// pointer register.  This is true if the function has variable sized allocas or
// if frame pointer elimination is disabled.
bool CDMFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MFI.hasVarSizedObjects() || MFI.isFrameAddressTaken();
}

void CDMFrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const CDMInstrInfo *TII = STI.getInstrInfo();

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = DebugLoc();

  // First, check if stack frame size exceeds limit
  ensureStackFrameAddressable(MF);

  // Second, compute final stack size.
  uint64_t StackSize = MFI.getStackSize();

  if (hasFP(MF)) {
    BuildMI(MBB, MBBI, DL, TII->get(CDM::PUSH)).addReg(CDM::FP);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::LDSP), CDM::FP);
  }

  if (StackSize == 0) {
    return;
  }

  // 4 instruction in alternative scheme
  const uint64_t StackSizeThreshold = 4 * 1024;

  if (StackSize > StackSizeThreshold) {
    BuildMI(MBB, MBBI, DL, TII->get(CDM::PUSH)).addReg(CDM::R0);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::ldi), CDM::R0).addImm(-StackSize + 2);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::ADDSPR)).addReg(CDM::R0);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::lsw), CDM::R0).addImm(0);

    return;
  }

  TII->adjustStackPtr(-StackSize, MBB, MBBI, DL);
}

void CDMFrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getFirstTerminator();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  const CDMInstrInfo *TII = STI.getInstrInfo();

  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI.getStackSize();

  if (!hasFP(MF) && StackSize != 0) {
    TII->adjustStackPtr(StackSize, MBB, MBBI, DL);
  }

  if (hasFP(MF)) {
    BuildMI(MBB, MBBI, DL, TII->get(CDM::STSP)).addReg(CDM::FP);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::POP), CDM::FP);
  }
}

MachineBasicBlock::iterator CDMFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {
  int Size = MI->getOperand(0).getImm();
  if (MI->getOpcode() == CDM::ADJCALLSTACKDOWN)
    Size = -Size;

  const auto *TII =
      static_cast<const CDMInstrInfo *>(MF.getSubtarget().getInstrInfo());

  DebugLoc DL = MI != MBB.end() ? MI->getDebugLoc() : DebugLoc();

  if (Size)
    TII->adjustStackPtr(Size, MBB, MI, DL);
  return MBB.erase(MI);
}
} // namespace llvm
