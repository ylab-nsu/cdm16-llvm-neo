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
  Invalid = -1,
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

/// Converts a string to a CondOp.
inline CondOp stringToCond(StringRef String) {
  return StringSwitch<CondOp>(String.lower())
      .Case("eq", EQ)
      .Case("ne", NE)
      .Case("z", Z)
      .Case("nz", NZ)
      .Case("hs", HS)
      .Case("lo", LO)
      .Case("cs", CS)
      .Case("cc", CC)
      .Case("mi", MI)
      .Case("pl", PL)
      .Case("vs", VS)
      .Case("vc", VC)
      .Case("hi", HI)
      .Case("ls", LS)
      .Case("ge", GE)
      .Case("lt", LT)
      .Case("gt", GT)
      .Case("le", LE)
      .Case("true", TRUE)
      .Case("false", FALSE)
      .Default(CDMCOND::Invalid);
}

/// Converts CondOp to its string representation.
inline StringRef condtoString(CondOp Cond) {
  switch (Cond) {
  default:
    llvm_unreachable("Unknown condition code");
  case EQ:
    return "eq";
  case NE:
    return "ne";
  case HS:
    return "hs";
  case LO:
    return "lo";
  case MI:
    return "mi";
  case PL:
    return "pl";
  case VS:
    return "vs";
  case VC:
    return "vc";
  case HI:
    return "hi";
  case LS:
    return "ls";
  case GE:
    return "ge";
  case LT:
    return "lt";
  case GT:
    return "gt";
  case LE:
    return "le";
  case TRUE:
    return "true";
  case FALSE:
    return "false";
  }
}

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

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, Register DestReg, Register SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  void adjustStackPtr(int64_t Amount, MachineBasicBlock &MBB,
                      MachineBasicBlock::iterator I, const DebugLoc &DL) const;

  bool expandPostRAPseudo(MachineInstr &MI) const override;

private:
  const CDMRegisterInfo RI;

  void expandBCond(MachineBasicBlock &MBB, MachineInstr &MI) const;
  void expandShiftExt(MachineBasicBlock &MBB, MachineInstr &MI) const;
};

} // namespace llvm

#endif // LLVM_CDMINSTRINFO_H
