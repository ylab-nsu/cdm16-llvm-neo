//
// Created by ilya on 21.11.23.
//

#define DEBUG_TYPE "cdm-reg-info"

#include "CDMFrameLowering.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/ModuleSummaryIndex.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/BranchProbability.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <iterator>
#include <map>

#include "CDMRegisterInfo.h"

#define GET_REGINFO_TARGET_DESC
#include "CDMGenRegisterInfo.inc"
#include "CDMInstrInfo.h"

namespace llvm {

CDMRegisterInfo::CDMRegisterInfo() : CDMGenRegisterInfo(CDM::PC) {}

BitVector CDMRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  static const std::array ReservedCPURegs{CDM::FP, CDM::SP, CDM::PSR, CDM::PC};

  for (unsigned I = 0; I < ReservedCPURegs.size(); ++I)
    Reserved.set(ReservedCPURegs[I]);

  return Reserved;
}

const MCPhysReg *
CDMRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  switch (MF->getFunction().getCallingConv()) {
  case CallingConv::C:
  case CallingConv::Fast:
  case CallingConv::Cold:
    return CSR_O16_SaveList;
  case CallingConv::CdmIsr:
    return CSR_O16_ALL_SaveList;
  }
  llvm_unreachable("Unknown calling convention");
}

bool CDMRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned int FIOperandNum,
                                          RegScavenger *RS) const {
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  const TargetInstrInfo *InstrInfo = MF.getSubtarget().getInstrInfo();
  MachineBasicBlock &MBB = *MI.getParent();

  unsigned I = 0;
  while (!MI.getOperand(I).isFI()) {
    ++I;
    assert(I < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  LLVM_DEBUG(errs() << "\nFunction : " << MF.getFunction().getName() << "\n";
             errs() << "<--------->\n"
                    << MI);

  int FrameIndex = MI.getOperand(I).getIndex();
  uint64_t StackSize = MF.getFrameInfo().getStackSize();
  int64_t FpOffset = MF.getFrameInfo().getObjectOffset(FrameIndex);

  LLVM_DEBUG(errs() << "FrameIndex : " << FrameIndex << "\n"
                    << "FpOffset   : " << FpOffset << "\n"
                    << "StackSize  : " << StackSize << "\n");

  MI.getOperand(I).ChangeToImmediate(FpOffset);

  // devil: my child will write switch-case like "they" want him to
  // god: abuse c++ till the day you die son

  // Opcode -> (Opcode, MemSize)
  static const std::map<unsigned, std::pair<unsigned, unsigned>>
      SubstitutionOpcsTable = {
          {CDM::ssw, {CDM::stwRR, 2}},   {CDM::lsw, {CDM::ldwRR, 2}},
          {CDM::ssb, {CDM::stbRR, 1}},   {CDM::lsb, {CDM::ldbRR, 1}},
          {CDM::lssb, {CDM::ldsbRR, 1}},
      };

  const auto OpcodeFind =
      std::map<unsigned, std::pair<unsigned, unsigned>>::const_iterator(
          SubstitutionOpcsTable.find(
              MI.getOpcode())); // because inline gods said so

  if (OpcodeFind == SubstitutionOpcsTable.end())
    return false; // no instruction to change

  auto [SubstitutionOpc, MemSize] = OpcodeFind->second;

  if ((FpOffset > MemSize * 64 - 1) || (FpOffset < MemSize * -64)) {
    const MachineOperand &SrcOperand = MI.getOperand(0);

    Register OffsetReg;
    // if load
    if (InstrInfo->get(MI.getOpcode()).mayLoad()) {
      OffsetReg = SrcOperand.getReg(); // just use same register since load will
                                       // re-define it anyway
    } else {
      OffsetReg = huntRegister(MBB, CDM::CPURegsRegClass, MI, true);
      if (!OffsetReg) // should not happen, but report in case a bug occurs
        report_fatal_error("Couldn't find register to use");
    }

    BuildMI(MBB, II, II->getDebugLoc(), InstrInfo->get(CDM::ldi), OffsetReg)
        .addImm(FpOffset);

    BuildMI(MBB, II, II->getDebugLoc(), InstrInfo->get(SubstitutionOpc))
        .add(SrcOperand)
        .addReg(OffsetReg, RegState::Kill)
        .addReg(MF.getSubtarget().getRegisterInfo()->getFrameRegister(MF))
        .addImm(0);

    MI.getParent()->erase(II);
    return true; // original instruction is removed
  }

  return false; // instruction not removed
}

Register CDMRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return CDM::FP;
}

const uint32_t *
CDMRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                      CallingConv::ID Id) const {
  switch (Id) {
  case CallingConv::C:
  case CallingConv::Fast:
  case CallingConv::Cold:
    return CSR_O16_RegMask;
  case CallingConv::CdmIsr:
    return CSR_O16_ALL_RegMask;
  }
  llvm_unreachable("Unknown calling convention");
}

Register CDMRegisterInfo::huntRegister(MachineBasicBlock &MBB,
                                       const TargetRegisterClass &RC,
                                       MachineBasicBlock::iterator MI,
                                       bool AllowStackAdj) {
  // TODO: handle inital sets (Used, GPR) thru info from tablegen (and put that
  // info into fucking tablegen in a first place)
  assert(MI->getParent() == &MBB && "MI must belong to the MBB");
  assert(MI != MBB.end() && "MI must not be MBB.end()");

  // r4-r6 are callee saved, so we can't initially asume they're available
  SmallSet<Register, CDM::NUM_TARGET_REGS> Used{CDM::R4, CDM::R5, CDM::R6};

  for (auto It = MBB.liveout_begin(); It != MBB.liveout_end(); ++It)
    Used.insert(It->PhysReg);

  for (auto It = std::prev(MBB.end()); It != MI; --It) {
    for (MachineOperand Op : It->operands()) {
      if (Op.isReg() && Op.getReg().isPhysical() && Op.isDef())
        Used.erase(Op.getReg());
    }

    for (MachineOperand Op : It->operands()) {
      if (Op.isReg() && Op.getReg().isPhysical() && Op.isUse())
        Used.insert(Op.getReg());
    }
  }

  SmallSet<Register, CDM::NUM_TARGET_REGS> LiveInMI;
  for (MachineOperand Op : MI->operands()) {
    if (Op.isReg() && Op.getReg().isPhysical())
      LiveInMI.insert(Op.getReg());
  }

  const SmallSet<Register, 7> GPR{CDM::R0, CDM::R1, CDM::R2, CDM::R3,
                                  CDM::R4, CDM::R5, CDM::R6};
  auto Available = GPR;

  for (Register Reg : Used)
    Available.erase(Reg);
  for (Register Reg : LiveInMI)
    Available.erase(Reg);

  if (!Available.empty())
    return *Available.begin();

  if (!AllowStackAdj)
    return 0;

  for (Register Reg : GPR) {
    if (!LiveInMI.contains(Reg)) {
      const auto *InstrInfo = MBB.getParent()->getSubtarget().getInstrInfo();
      BuildMI(MBB, MI, MI->getDebugLoc(), InstrInfo->get(CDM::PUSH))
          .addReg(Reg, RegState::Kill);
      BuildMI(MBB, std::next(MI), MI->getDebugLoc(), InstrInfo->get(CDM::POP),
              Reg);

      return Reg;
    }
  }

  return 0;
}

} // namespace llvm
