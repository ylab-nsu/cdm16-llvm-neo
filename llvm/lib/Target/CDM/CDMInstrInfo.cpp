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
  case CDM::SHL_PARTS_1:
  case CDM::SRL_PARTS_1:
  case CDM::SRA_PARTS_1:
    expandShiftParts(MBB, MI);
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

void CDMInstrInfo::expandShiftParts(MachineBasicBlock &MBB,
                                    MachineInstr &MI) const {
  MachineFunction &MF = *MBB.getParent();
  DebugLoc DL = MI.getDebugLoc();

  Register DstLo = MI.getOperand(0).getReg();
  Register DstHi = MI.getOperand(1).getReg();
  Register SrcLo = MI.getOperand(2).getReg();
  Register SrcHi = MI.getOperand(3).getReg();

  unsigned LoOpc;
  unsigned HiOpc;
  bool LoFirst;
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("Unknown shift operation");
  case CDM::SHL_PARTS_1:
    LoOpc = CDM::SHL;
    HiOpc = CDM::RCL;
    LoFirst = true;
    break;
  case CDM::SRL_PARTS_1:
    LoOpc = CDM::RCR;
    HiOpc = CDM::SHR;
    LoFirst = false;
    break;
  case CDM::SRA_PARTS_1:
    LoOpc = CDM::RCR;
    HiOpc = CDM::SHRA;
    LoFirst = false;
    break;
  }

  MIBundleBuilder Bundler = MIBundleBuilder(MBB, MI);
  if (LoFirst) {
    Bundler.append(BuildMI(MF, DL, get(LoOpc), DstLo).addReg(SrcLo).addImm(1));
    Bundler.append(BuildMI(MF, DL, get(HiOpc), DstHi).addReg(SrcHi).addImm(1));
  } else {
    Bundler.append(BuildMI(MF, DL, get(HiOpc), DstHi).addReg(SrcHi).addImm(1));
    Bundler.append(BuildMI(MF, DL, get(LoOpc), DstLo).addReg(SrcLo).addImm(1));
  }
  finalizeBundle(MBB, Bundler.begin(), Bundler.end());
}

void CDMInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI,
                               const DebugLoc &DL, Register DestReg,
                               Register SrcReg, bool KillSrc,
                               bool RenamableDest, bool RenamableSrc) const {
  //  TargetInstrInfo::copyPhysReg(MBB, MI, DL, DestReg, SrcReg, KillSrc);
  if (SrcReg == CDM::SP) {
    MachineInstrBuilder MIB = BuildMI(MBB, MI, DL, get(CDM::LDSP));
    MIB.addReg(DestReg, RegState::Define);
    return;
  }
  assert(CDM::CPURegsRegClass.contains(SrcReg) &&
         CDM::CPURegsRegClass.contains(DestReg) &&
         "Can only copy General purpose regs");
  // TODO: check order
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
