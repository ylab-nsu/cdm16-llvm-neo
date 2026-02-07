//
// Created by Ilya Merzlyakov on 04.12.2023.
//

#include "CDMMCInstLower.h"
#include "CDMAsmPrinter.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

namespace llvm {
CDMMCInstLower::CDMMCInstLower(MCContext &C, CDMAsmPrinter &AsmPrinter)
    : Ctx(C), AsmPrinter(AsmPrinter) {}

void CDMMCInstLower::lower(const MachineInstr *MI, MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned I = 0, E = MI->getNumOperands(); I != E; I++) {
    const MachineOperand &MO = MI->getOperand(I);
    MCOperand MCOp = lowerOperand(MO);

    if (MCOp.isValid()) {
      OutMI.addOperand(MCOp);
    }
  }
}

MCOperand CDMMCInstLower::lowerOperand(const MachineOperand &MO) const {
  auto MOType = MO.getType();
  switch (MOType) {
  default:
    llvm_unreachable("Unknown operand type");
  case MachineOperand::MO_Register:
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm());
  case MachineOperand::MO_MachineBasicBlock:
    return lowerSymbolOperand(MO, MO.getMBB()->getSymbol(), 0);
  case MachineOperand::MO_GlobalAddress:
    return lowerSymbolOperand(MO, AsmPrinter.getSymbol(MO.getGlobal()),
                              MO.getOffset());
  case MachineOperand::MO_ExternalSymbol:
    return lowerSymbolOperand(
        MO, AsmPrinter.GetExternalSymbolSymbol(MO.getSymbolName()),
        MO.getOffset());
  case MachineOperand::MO_JumpTableIndex:
    return lowerSymbolOperand(MO, AsmPrinter.GetJTISymbol(MO.getIndex()), 0);
  case MachineOperand::MO_RegisterMask:
    break;
  }
  return MCOperand();
}

MCOperand CDMMCInstLower::lowerSymbolOperand(const MachineOperand &MO,
                                             MCSymbol *Symbol,
                                             int64_t Offset) const {
  const MCExpr *Expr = MCSymbolRefExpr::create(Symbol, Ctx);

  if (Offset > 0) {
    const MCExpr *OffExpr = MCConstantExpr::create(Offset, Ctx);
    Expr = MCBinaryExpr::createAdd(Expr, OffExpr, Ctx);
  } else if (Offset < 0) {
    const MCExpr *OffExpr = MCConstantExpr::create(-Offset, Ctx);
    Expr = MCBinaryExpr::createSub(Expr, OffExpr, Ctx);
  }

  return MCOperand::createExpr(Expr);
}

} // namespace llvm
