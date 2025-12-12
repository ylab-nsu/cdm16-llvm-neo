//
// Created by ilya on 16.10.23.
//

#ifndef LLVM_CDMASMPRINTER_H
#define LLVM_CDMASMPRINTER_H

#include "CDMMCInstLower.h"
#include "MCTargetDesc/CDMTargetStreamer.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {

class CDMAsmPrinter : public AsmPrinter {
  CDMMCInstLower MCInstLower;
  llvm::StringMap<int> SourceFiles;

  std::optional<int> getSourceFileIndex(StringRef Checksum);
  void collectAndEmitSourceFiles(Module &Module);

public:
  explicit CDMAsmPrinter(TargetMachine &TM,
                         std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MCInstLower(*this) {}
  void emitFunctionEntryLabel() override;

  StringRef getPassName() const override { return "CDM Assembly Printer"; }

  void emitInstruction(const MachineInstr *Instr) override;
  void emitFunctionBodyStart() override;
  void emitFunctionBodyEnd() override;
  void emitLinkage(const GlobalValue *GV, MCSymbol *GVSym) const override;
  void emitFunctionHeader() override;
  void emitStartOfAsmFile(Module &Module) override;
  void emitEndOfAsmFile(Module &Module) override;

  CDMTargetStreamer *getTargetStreamer() const { 
    return static_cast<CDMTargetStreamer *>(OutStreamer->getTargetStreamer()); 
  }
};

} // namespace llvm

#endif // LLVM_CDMASMPRINTER_H
