//
// Created by ilya on 21.11.23.
//

#include "CDMInstrInfo.h"

#include "CDM.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "CDMGenInstrInfo.inc"

namespace llvm {

CDMInstrInfo::CDMInstrInfo()
    : CDMGenInstrInfo(CDM::ADJCALLSTACKDOWN, CDM::ADJCALLSTACKUP) {}

// needed for loading/saving regs in prologue/epilogue
void CDMInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator I,
                                       Register SrcReg, bool IsKill, int FI,
                                       const TargetRegisterClass *RC,
                                       const TargetRegisterInfo *TRI,
                                       Register VReg,
                                       MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOStore);

  // TODO: understand this
  BuildMI(MBB, I, DL, get(CDM::ssw))
      .addReg(SrcReg, getKillRegState(IsKill))
      .addFrameIndex(FI)
      .addMemOperand(MMO);
}

// TODO: understand this
MachineMemOperand *
CDMInstrInfo::GetMemOperand(MachineBasicBlock &MBB, int FI,
                            MachineMemOperand::Flags Flags) const {
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  // Offset?
  return MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(MF, FI),
                                 Flags, MFI.getObjectSize(FI),
                                 MFI.getObjectAlign(FI));
}

void CDMInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator MI,
                                        Register DestReg, int FrameIndex,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI,
                                        Register VReg,
                                        MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  if (MI != MBB.end())
    DL = MI->getDebugLoc();
  MachineMemOperand *MMO =
      GetMemOperand(MBB, FrameIndex, MachineMemOperand::MOLoad);

  // OFFSET?
  BuildMI(MBB, MI, DL, get(CDM::lsw), DestReg)
      .addFrameIndex(FrameIndex)
      .addMemOperand(MMO);
}

bool CDMInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  MachineBasicBlock &MBB = *MI.getParent();

  switch (MI.getDesc().getOpcode()) {
  default:
    return false;
  case CDM::PseudoRet:
    expandRet(MBB, MI);
    break;
  case CDM::PseudoBCondRI:
  case CDM::PseudoBCondRR:
    expandBCond(MBB, MI);
    break;
  case CDM::SHL_EXT32:
  case CDM::SHR_EXT32:
  case CDM::SHRA_EXT32:
  case CDM::SHL_EXT64:
  case CDM::SHR_EXT64:
  case CDM::SHRA_EXT64:
    expandShiftExt(MBB, MI);
    break;
  }

  MBB.erase(MI);
  return true;
}

void CDMInstrInfo::expandRet(MachineBasicBlock &MBB, MachineInstr &MI) const {
  auto Opcode = CDM::rts;
  if (MBB.getParent()->getFunction().getCallingConv() == CallingConv::CdmIsr) {
    Opcode = CDM::rti;
  }
  BuildMI(MBB, MI, MI.getDebugLoc(), get(Opcode));
}

void CDMInstrInfo::expandBCond(MachineBasicBlock &MBB, MachineInstr &MI) const {

  DebugLoc DL = MI.getDebugLoc();

  auto CondCode = static_cast<CDMCOND::CondOp>(MI.getOperand(0).getImm());
  auto LHS = MI.getOperand(1);
  auto RHS = MI.getOperand(2);
  auto Target = MI.getOperand(3);

  // TODO: Implement optimizations

  MIBundleBuilder Bundler = MIBundleBuilder(MBB, MI);

  if (MI.getOpcode() == CDM::PseudoBCondRR) {
    Bundler.append(BuildMI(*MBB.getParent(), DL, get(CDM::CMP))
                       .addReg(LHS.getReg())
                       .addReg(RHS.getReg()));
  } else {
    Bundler.append(BuildMI(*MBB.getParent(), DL, get(CDM::CMPI))
                       .addReg(LHS.getReg())
                       .addImm(RHS.getImm()));
  }

  Bundler.append(BuildMI(*MBB.getParent(), DL, get(CDM::BCond))
                     .addImm(CondCode)
                     .addMBB(Target.getMBB()));

  finalizeBundle(MBB, Bundler.begin(), Bundler.end());
}

// Expands shift_EXT pseudos into shift-rotate chains
void CDMInstrInfo::expandShiftExt(MachineBasicBlock &MBB,
                                  MachineInstr &MI) const {
  MachineFunction &MF = *MBB.getParent();
  DebugLoc DL = MI.getDebugLoc();

  unsigned HeadOpc;
  unsigned TailOpc;
  int RegCount;
  bool HeadIsLo;
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("Unknown shift operation");
  case CDM::SHL_EXT32:
    HeadOpc = CDM::SHL;
    TailOpc = CDM::RCL;
    RegCount = 2;
    HeadIsLo = true;
    break;
  case CDM::SHR_EXT32:
    HeadOpc = CDM::SHR;
    TailOpc = CDM::RCR;
    RegCount = 2;
    HeadIsLo = false;
    break;
  case CDM::SHRA_EXT32:
    HeadOpc = CDM::SHRA;
    TailOpc = CDM::RCR;
    RegCount = 2;
    HeadIsLo = false;
    break;
  case CDM::SHL_EXT64:
    HeadOpc = CDM::SHL;
    TailOpc = CDM::RCL;
    RegCount = 4;
    HeadIsLo = true;
    break;
  case CDM::SHR_EXT64:
    HeadOpc = CDM::SHR;
    TailOpc = CDM::RCR;
    RegCount = 4;
    HeadIsLo = false;
    break;
  case CDM::SHRA_EXT64:
    HeadOpc = CDM::SHRA;
    TailOpc = CDM::RCR;
    RegCount = 4;
    HeadIsLo = false;
    break;
  }

  SmallVector<Register, 4> DstRegs;
  SmallVector<Register, 4> SrcRegs;
  for (int I = 0; I < RegCount; I++) {
    DstRegs.push_back(MI.getOperand(I).getReg());
  }
  for (int I = 0; I < RegCount; I++) {
    SrcRegs.push_back(MI.getOperand(RegCount + I).getReg());
  }

  // Bundle up the chain because a move inserted between its elements may break
  // it.
  MIBundleBuilder Bundler = MIBundleBuilder(MBB, MI);
  if (HeadIsLo) {
    Bundler.append(
        BuildMI(MF, DL, get(HeadOpc), DstRegs[0]).addReg(SrcRegs[0]).addImm(1));
    for (int I = 1; I < RegCount; I++) {
      Bundler.append(BuildMI(MF, DL, get(TailOpc), DstRegs[I])
                         .addReg(SrcRegs[I])
                         .addImm(1));
    }
  } else {
    Bundler.append(BuildMI(MF, DL, get(HeadOpc), DstRegs[RegCount - 1])
                       .addReg(SrcRegs[RegCount - 1])
                       .addImm(1));
    for (int I = RegCount - 2; I >= 0; I--) {
      Bundler.append(BuildMI(MF, DL, get(TailOpc), DstRegs[I])
                         .addReg(SrcRegs[I])
                         .addImm(1));
    }
  }
  finalizeBundle(MBB, Bundler.begin(), Bundler.end());
}

void CDMInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI,
                               const DebugLoc &DL, Register DestReg,
                               Register SrcReg, bool KillSrc,
                               bool RenamableDest, bool RenamableSrc) const {
  if (SrcReg == CDM::SP) {
    assert(CDM::CPURegsRegClass.contains(DestReg) &&
           "Cannot copy SP to special register");

    MachineInstrBuilder MIB = BuildMI(MBB, MI, DL, get(CDM::LDSP));
    MIB.addReg(DestReg, RegState::Define);
    return;
  }
  if (DestReg == CDM::SP) {
    assert(CDM::CPURegsRegClass.contains(SrcReg) &&
           "Cannot copy a special register to SP");

    MachineInstrBuilder MIB = BuildMI(MBB, MI, DL, get(CDM::STSP));
    MIB.addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }

  assert(CDM::CPURegsRegClass.contains(SrcReg) &&
         CDM::CPURegsRegClass.contains(DestReg) &&
         "Impossible reg-to-reg copy");
  MachineInstrBuilder MIB = BuildMI(MBB, MI, DL, get(CDM::MOVE));
  MIB.addReg(DestReg);
  MIB.addReg(SrcReg, getKillRegState(KillSrc));
}

void CDMInstrInfo::adjustStackPtr(int64_t Amount, MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL) const {
  BuildMI(MBB, I, DL, get(CDM::ADDSP)).addImm(Amount);
}

} // namespace llvm
