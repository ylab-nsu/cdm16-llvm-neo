//
// Created by ilya on 16.10.23.
//

#include "CDMInstPrinter.h"
#include <map>

#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
#define PRINT_ALIAS_INSTR
#include "CDMGenAsmWriter.inc"
#include "CDMInstrInfo.h"
#include "MCTargetDesc/CDMAsmStreamer.h"

void CDMInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  // TODO: printAliasInst()?
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
    O << Op.getImm();
    return;
  }
  if (Op.isExpr()) {
    CDMAsmStreamer::printExpr(O, *Op.getExpr());
    return;
  }

  llvm_unreachable("Unknown operand type");
}

void CDMInstPrinter::printMemOperand(const MCInst *MI, unsigned int OpNo,
                                     raw_ostream &O) {
  if (MI->getOperand(OpNo + 1).isImm()) {
    assert(MI->getOperand(OpNo + 1).getImm() == 0 &&
           "Mem operand can't have non-zero offset");
  }
  printOperand(MI, OpNo, O);
}
void CDMInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
  OS << StringRef(const_cast<CDMInstPrinter *>(this)->getRegisterName(Reg));
}
void CDMInstPrinter::printCondCode(const MCInst *MI, unsigned int OpNo,
                                   raw_ostream &O) {
  using namespace llvm;
  std::map<CDMCOND::CondOp, std::string> CondMap = {
      {CDMCOND::LT, "lt"}, {CDMCOND::LE, "le"}, {CDMCOND::GT, "gt"},
      {CDMCOND::GE, "ge"}, {CDMCOND::LO, "lo"}, {CDMCOND::LS, "ls"},
      {CDMCOND::HI, "hi"}, {CDMCOND::HS, "hs"}, {CDMCOND::EQ, "eq"},
      {CDMCOND::NE, "ne"},
  };
  O << CondMap.at((CDMCOND::CondOp)MI->getOperand(OpNo).getImm());
}
void CDMInstPrinter::printMemRROperand(const MCInst *MI, unsigned int OpNo,
                                       raw_ostream &O) {
  printOperand(MI, OpNo, O);
  O << ", ";
  printOperand(MI, OpNo + 1, O);
}
