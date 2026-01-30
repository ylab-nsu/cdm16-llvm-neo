//
// Created by ilya on 21.11.23.
//

#ifndef LLVM_CDMINSTRINFO_H
#define LLVM_CDMINSTRINFO_H

#include "CDMRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "CDMGenInstrInfo.inc"
#include "llvm/CodeGen/ISDOpcodes.h"

namespace llvm {

namespace CDMCOND {
enum CondOp {
  EQ = 0x0,    // equal
  NE = 0x1,    // not equal
  Z = EQ,      // zero
  NZ = NE,     // not zero
  HS = 0x2,    // unsigned greater or equal
  LO = 0x3,    // unsigned less
  CS = HS,     // carry flag is set
  CC = LO,     // carry flag is not set
  MI = 0x4,    // negative
  PL = 0x5,    // positive
  VS = 0x6,    // overflow flag is set
  VC = 0x7,    // overflow flag is not set
  HI = 0x8,    // unsigned greater
  LS = 0x9,    // unsigned less than or equal
  GE = 0xA,    // signed greater or equal
  LT = 0xB,    // signed less than
  GT = 0xC,    // signed greater
  LE = 0xD,    // signed less than or equal
  TRUE = 0xE,  // always true
  FALSE = 0xF, // always false
};

} // namespace CDMCOND

class CDMInstrInfo : public CDMGenInstrInfo {
public:
  explicit CDMInstrInfo();

  const CDMRegisterInfo &getRegisterInfo() const { return RI; }
  void storeRegToStackSlot(
      MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register SrcReg,
      bool IsKill, int FI, const TargetRegisterClass *RC,
      const TargetRegisterInfo *TRI, Register VReg,
      MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;
  void loadRegFromStackSlot(
      MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg,
      int FrameIndex, const TargetRegisterClass *RC,
      const TargetRegisterInfo *TRI, Register VReg,
      MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;

  bool expandPostRAPseudo(MachineInstr &MI) const override;
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, Register DestReg, Register SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  void adjustStackPtr(int64_t Amount, MachineBasicBlock &MBB,
                      MachineBasicBlock::iterator I, const DebugLoc &DL) const;

private:
  const CDMRegisterInfo RI;

  MachineMemOperand *GetMemOperand(MachineBasicBlock &MBB, int FI,
                                   MachineMemOperand::Flags Flags) const;

  void expandRet(MachineBasicBlock &MBB, MachineInstr &MI) const;
  void expandBCond(MachineBasicBlock &MBB, MachineInstr &MI) const;
  void expandShiftExt(MachineBasicBlock &MBB, MachineInstr &MI) const;
};

} // namespace llvm

#endif // LLVM_CDMINSTRINFO_H
