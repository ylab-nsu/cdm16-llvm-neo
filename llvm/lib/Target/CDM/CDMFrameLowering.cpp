//
// Created by ilya on 28.10.23.
//

#include "CDMFrameLowering.h"

#include "CDMFunctionInfo.h"
#include "CDMInstrInfo.h"
#include "CDMSubtarget.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/ADT/SmallSet.h"
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

// It is better to use an alternative algorithm to adjust the stack pointer
// in the prologue when the frame is larger than this number of bytes.
constexpr int64_t BigFrameThreshold = 4 * 1024;

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

// FP is disabled only if the function doesn't use the stack at all.
// We don't have proper SP-relative addressing, so we have no choice, but to use
// FP.
bool CDMFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const CDMFunctionInfo &TFI = *MF.getInfo<CDMFunctionInfo>();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MFI.isFrameAddressTaken() || MFI.hasVarSizedObjects() ||
         TFI.getUsesFrameIndices() ||
         TFI.getEstimatedFrameSize() > BigFrameThreshold;
}

bool CDMFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return !MFI.hasVarSizedObjects();
}

void CDMFrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const CDMFunctionInfo &TFI = *MF.getInfo<CDMFunctionInfo>();
  const CDMInstrInfo *TII = STI.getInstrInfo();

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = DebugLoc();

  // Check if stack frame size exceeds limit
  ensureStackFrameAddressable(MF);

  // Skip CSR pushes from spillCalleeSavedRegisters
  while ((MBBI != MBB.end()) && MBBI->getFlag(MachineInstr::FrameSetup) &&
         (MBBI->getOpcode() == CDM::PUSH)) {
    ++MBBI;
  }

  // FP points to the last callee saved register stored with CDM::PUSH.
  // CSRs are included in the frame (contiguous area ending at offset 0).
  if (hasFP(MF)) {
    BuildMI(MBB, MBBI, DL, TII->get(CDM::LDSP), CDM::FP);
    // Mark FP as live-in in every block except the entry.
    for (MachineBasicBlock &MBBJ : llvm::drop_begin(MF)) {
      MBBJ.addLiveIn(CDM::FP);
    }
  }

  // Callee saves have already adjusted the stack pointer partially,
  // so subtract their size from the total frame size.
  uint64_t StackSize = MFI.getStackSize() - TFI.getCaleeSavedSize();

  if (hasFP(MF) && StackSize > BigFrameThreshold) {
    // Alternative algorithm for large frames that relies on FP
    BuildMI(MBB, MBBI, DL, TII->get(CDM::PUSH)).addReg(CDM::R6);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::LDIImm16), CDM::R6)
        .addImm(-StackSize + 2);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::ADDSPReg)).addReg(CDM::R6);
    BuildMI(MBB, MBBI, DL, TII->get(CDM::LSW), CDM::R6).addImm(0);
  } else {
    TII->adjustStackPtr(-StackSize, MBB, MBBI, DL);
  }
}

void CDMFrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  const CDMFunctionInfo &TFI = *MF.getInfo<CDMFunctionInfo>();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const CDMInstrInfo *TII = STI.getInstrInfo();

  MachineBasicBlock::iterator MBBI = MBB.getFirstTerminator();
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Unwind to before CSR pops from restoreCalleeSavedRegisters
  while (MBBI != MBB.begin()) {
    MachineBasicBlock::iterator PI = std::prev(MBBI);
    int Opc = PI->getOpcode();

    if (Opc != CDM::POP && !PI->isTerminator()) {
      break;
    }

    --MBBI;
  }

  // See emitPrologue
  uint64_t StackSize = MFI.getStackSize() - TFI.getCaleeSavedSize();

  if (hasFP(MF)) {
    BuildMI(MBB, MBBI, DL, TII->get(CDM::STSP)).addReg(CDM::FP, RegState::Kill);
  } else {
    TII->adjustStackPtr(StackSize, MBB, MBBI, DL);
  }
}

bool CDMFrameLowering::assignCalleeSavedSpillSlots(
    MachineFunction &MF, const TargetRegisterInfo *TRI,
    std::vector<CalleeSavedInfo> &CSI) const {
  CDMFunctionInfo &TFI = *MF.getInfo<CDMFunctionInfo>();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  int64_t Offset = 0;
  for (CalleeSavedInfo &I : CSI) {
    // Calee saved spill slots are always allocated in a contiguous area
    // at the start of the frame.
    Offset += 2;
    int FI = MFI.CreateFixedObject(2, -Offset, true);
    I.setFrameIdx(FI);
  }
  TFI.setCaleeSavedSize(Offset);

  return true;
}

void CDMFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                            BitVector &SavedRegs,
                                            RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  CDMFunctionInfo &TFI = *MF.getInfo<CDMFunctionInfo>();

  if (TFI.isISRWithContext()) {
    // ISRs that accept a pointer to the previous context need all registers
    // saved. Order is guaranteed by the CSR save list defined in
    // CDMCallingConv.td
    for (unsigned Reg : {CDM::FP, CDM::R6, CDM::R5, CDM::R4, CDM::R3, CDM::R2,
                         CDM::R1, CDM::R0}) {
      SavedRegs.set(Reg);
    }
  } else if (hasFP(MF)) {
    // Previous value of FP needs to be saved if the function uses it.
    SavedRegs.set(CDM::FP);
  }
}

bool CDMFrameLowering::spillCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    ArrayRef<CalleeSavedInfo> CSI, const TargetRegisterInfo *TRI) const {
  if (CSI.empty()) {
    return false;
  }

  DebugLoc DL = MBB.findDebugLoc(MI);
  MachineFunction &MF = *MBB.getParent();
  const CDMSubtarget &STI = MF.getSubtarget<CDMSubtarget>();
  const TargetInstrInfo &TII = *STI.getInstrInfo();

  for (const CalleeSavedInfo &I : llvm::reverse(CSI)) {
    MCRegister Reg = I.getReg();

    BuildMI(MBB, MI, DL, TII.get(CDM::PUSH))
        .addReg(Reg, RegState::Kill) // Kill the register, since the previous
                                     // value is not used anymore.
        .setMIFlag(MachineInstr::FrameSetup);
  }

  return true;
}

bool CDMFrameLowering::restoreCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    MutableArrayRef<CalleeSavedInfo> CSI, const TargetRegisterInfo *TRI) const {
  if (CSI.empty()) {
    return false;
  }

  DebugLoc DL = MBB.findDebugLoc(MI);
  const MachineFunction &MF = *MBB.getParent();
  const CDMSubtarget &STI = MF.getSubtarget<CDMSubtarget>();
  const TargetInstrInfo &TII = *STI.getInstrInfo();

  for (const CalleeSavedInfo &CCSI : CSI) {
    MCRegister Reg = CCSI.getReg();
    BuildMI(MBB, MI, DL, TII.get(CDM::POP), Reg);
  }

  return true;
}

MachineBasicBlock::iterator CDMFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {
  // Ignore the pseudo if the function frame has a space reserved for passing
  // stack arguments.
  if (!hasReservedCallFrame(MF)) {
    int Size = MI->getOperand(0).getImm();
    if (MI->getOpcode() == CDM::PseudoCallAdjustStackDown)
      Size = -Size;
    const auto *TII =
        static_cast<const CDMInstrInfo *>(MF.getSubtarget().getInstrInfo());
    DebugLoc DL = MI != MBB.end() ? MI->getDebugLoc() : DebugLoc();
    if (Size) {
      TII->adjustStackPtr(Size, MBB, MI, DL);
    }
  }
  return MBB.erase(MI);
}

/// This pass determines if any instructions in the function use any frame
/// indices. It also estimates and records the frame size.
/// This information is used in TargetFrameLowering::hasFPImpl().
///
/// Note: we can't just check MFI.getNumObjects() == 0 in
/// TargetFrameLowering::hasFPImpl(), because it is called multiple times during
/// the PEI pass. A new stack object may be allocated in the middle of PEI (e.g.
/// callee saved register slots), causing the value of
/// TargetFrameLowering::hasFPImpl() to change, potentially desynchronising
/// parts of PEI.
struct CDMFrameAnalyzer : public MachineFunctionPass {
  static char ID;
  CDMFrameAnalyzer() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    const MachineFrameInfo &MFI = MF.getFrameInfo();
    CDMFunctionInfo &TFI = *MF.getInfo<CDMFunctionInfo>();

    TFI.setEstimatedFrameSize(MFI.estimateStackSize(MF));

    if (MFI.getNumObjects() == 0) {
      // Short-circuit if we don't have any objects at all
      return false;
    }

    // These are the only instructions that can have frame index operands.
    static const SmallSet<unsigned, 5> FPRelPseudos = {
        CDM::PseudoStoreFrameWord,    CDM::PseudoLoadFrameWord,
        CDM::PseudoStoreFrameByte,    CDM::PseudoLoadFrameByteZext,
        CDM::PseudoLoadFrameByteSext, CDM::PseudoFrameAddress};

    for (const MachineBasicBlock &BB : MF) {
      for (const MachineInstr &MI : BB) {
        int Opcode = MI.getOpcode();
        if (FPRelPseudos.contains(Opcode)) {
          TFI.setUsesFrameIndices(true);
          return false;
        }
      }
    }
    return false;
  }

  StringRef getPassName() const override { return "CDM Frame Analyzer"; }
};

char CDMFrameAnalyzer::ID = 0;

/// Creates instance of the frame analyzer pass.
FunctionPass *createCDMFrameAnalyzerPass() { return new CDMFrameAnalyzer(); }

} // namespace llvm
