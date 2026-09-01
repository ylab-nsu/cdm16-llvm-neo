//
// Created by Ilya Merzlyakov on 04.12.2023.
//

#ifndef LLVM_CDMMCINSTLOWER_H
#define LLVM_CDMMCINSTLOWER_H

#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCContext.h"

namespace llvm {
class AsmPrinter;

class CDMMCInstLower {
public:
  CDMMCInstLower(MCContext &C, AsmPrinter &AsmPrinter);
  void lower(const MachineInstr *MI, MCInst &OutMI) const;
  MCOperand lowerOperand(const MachineOperand &MO) const;
  MCOperand lowerSymbolOperand(MCSymbol *Symbol, int64_t Offset) const;

private:
  MCContext &Ctx;
  AsmPrinter &Printer;
};

} // namespace llvm

#endif // LLVM_CDMMCINSTLOWER_H
