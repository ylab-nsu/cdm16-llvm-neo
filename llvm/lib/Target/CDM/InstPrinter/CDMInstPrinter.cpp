//
// Created by ilya on 16.10.23.
//

#include "CDMInstPrinter.h"
#include "CDMInstrInfo.h"
#include "MCTargetDesc/CDMAsmStreamer.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"

#include <map>

using namespace llvm;
#define PRINT_ALIAS_INSTR
#include "CDMGenAsmWriter.inc"

void CDMInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  if (!printAliasInstr(MI, Address, O))
    printInstruction(MI, Address, O);
  printAnnotation(O, Annot);
}

void CDMInstPrinter::printOperand(const MCInst *MI, unsigned int OpNo,
                                  raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    markup(O, Markup::Immediate) << formatImm(Op.getImm());
    return;
  }
  if (Op.isExpr()) {
    CDMAsmStreamer::printExpr(O, *Op.getExpr());
    return;
  }

  llvm_unreachable("Unknown operand type");
}

void CDMInstPrinter::printCondCode(const MCInst *MI, unsigned int OpNo,
                                   raw_ostream &O) {
  using namespace llvm;
  CDMCOND::CondOp Cond = (CDMCOND::CondOp)MI->getOperand(OpNo).getImm();
  O << CDMCOND::condtoString(Cond);
}

void CDMInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
  markup(OS, Markup::Register)
      << StringRef(const_cast<CDMInstPrinter *>(this)->getRegisterName(Reg));
}

void CDMInstPrinter::printMem2RegOperand(const MCInst *MI, unsigned int OpNo,
                                         raw_ostream &O) {
  printOperand(MI, OpNo, O);
  O << ", ";
  printOperand(MI, OpNo + 1, O);
}

void CDMInstPrinter::printPCRelBranchOperand(const MCInst *MI, uint64_t Address,
                                             unsigned OpNo, raw_ostream &O) {
  if (SymbolizeOperands) {
    return;
  }

  const MCOperand &Op = MI->getOperand(OpNo);
  if (!Op.isImm()) {
    return printOperand(MI, OpNo, O);
  }

  uint64_t Target = static_cast<uint64_t>(Op.getImm()) & 0xFFFF;
  markup(O, Markup::Target) << formatHex(Target);
}

void CDMInstPrinter::printBranchOperand(const MCInst *MI, unsigned OpNo,
                                        raw_ostream &O) {
  if (SymbolizeOperands) {
    return;
  }

  const MCOperand &Op = MI->getOperand(OpNo);
  if (!Op.isImm()) {
    return printOperand(MI, OpNo, O);
  }

  uint64_t Target = static_cast<uint64_t>(Op.getImm()) & 0xFFFF;
  markup(O, Markup::Target) << formatHex(Target);
}
