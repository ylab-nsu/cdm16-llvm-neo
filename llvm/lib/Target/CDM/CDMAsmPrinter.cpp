//
// Created by ilya on 16.10.23.
//

#include "CDMAsmPrinter.h"
#include "MCTargetDesc/CDMTargetStreamer.h"
#include "TargetInfo/CDMTargetInfo.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetOptions.h"
#include <set>

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCDMAsmPrinter() {
  RegisterAsmPrinter<CDMAsmPrinter> X(getTheCDMTarget());
}

std::optional<int> CDMAsmPrinter::getSourceFileIndex(StringRef Checksum) {
  if (this->SourceFiles.contains(Checksum)) {
    return this->SourceFiles.lookup(Checksum);
  }

  return std::nullopt;
}

void CDMAsmPrinter::collectAndEmitSourceFiles(Module &Module) {
  for (Function &Function : Module) {
    for (BasicBlock &BasicBlock : Function) {
      for (Instruction &Instruction : BasicBlock) {
        DILocation *DebugLoc = Instruction.getDebugLoc().get();

        if (DebugLoc) {
          StringRef Checksum = DebugLoc->getFile()->getChecksum().value().Value;

          StringRef DirRef = DebugLoc->getFile()->getDirectory();
          StringRef FileRef = DebugLoc->getFile()->getFilename();

          const Twine &Dir =
              Twine(llvm::sys::path::remove_leading_dotslash(DirRef));
          const Twine &File =
              Twine(llvm::sys::path::remove_leading_dotslash(FileRef));

          std::string RawPath = Dir.concat("/").concat(File).str();

          std::string Path = llvm::sys::path::convert_to_slash(RawPath);

          // место для dbg_source

          if (!getSourceFileIndex(Checksum)) {
            OutStreamer->emitRawText(formatv("dbg_source {0}, \"{1}\"\n",
                                             this->SourceFiles.size(), Path));
            this->SourceFiles.insert({Checksum, this->SourceFiles.size()});
          }
        }
      }
    }
  }
}

void CDMAsmPrinter::emitInstruction(const MachineInstr *Instr) {
  static unsigned PrevLineNumber = 0;
  static int PrevFileIndex = -1;

  if (Instr->isDebugValue()) {
    // TODO: implement
    return;
  }
  if (Instr->isDebugLabel())
    return;


  // место для dbg_loc

  DILocation *DebugLoc = Instr->getDebugLoc().get();

  if (DebugLoc) {
    StringRef Checksum = DebugLoc->getFile()->getChecksum().value().Value;

    std::optional<int> SourceFileIndex = getSourceFileIndex(Checksum);

    if (SourceFileIndex) {
      unsigned CurrLineNumber = DebugLoc->getLine(),
               CurrColumnNumber = DebugLoc->getColumn();

      if (PrevLineNumber != CurrLineNumber ||
          PrevFileIndex != SourceFileIndex) {

        OutStreamer->getCommentOS()
            << formatv("{0}:{1}:{2}", DebugLoc->getFilename(), CurrLineNumber,
                       CurrColumnNumber)
            << "\n";

        OutStreamer->emitRawText(formatv("\n\tdbg_loc {0}, {1}, {2}",
                                         *SourceFileIndex, CurrLineNumber,
                                         CurrColumnNumber));

        PrevLineNumber = CurrLineNumber;
        PrevFileIndex = *SourceFileIndex;
      }
    }
  }

  MachineBasicBlock::const_instr_iterator I = Instr->getIterator();
  MachineBasicBlock::const_instr_iterator E = Instr->getParent()->instr_end();

  // If instruction we emit is actually inside an instruction bundle,
  // iterate over all instructions in bundle and emit them all
  do {
    // Skip bundle pseudo instruction and emit content of a bundle
    if (I->isBundle()){
	continue;
    }

    if (I->isPseudo())
      llvm_unreachable("Pseudo opcode found in emitInstruction()");

    MCInst TmpInst0;
    MCInstLower.lower(&*I, TmpInst0);
    OutStreamer->emitInstruction(TmpInst0, getSubtargetInfo());
  } while ((++I != E) && I->isInsideBundle());
}

void CDMAsmPrinter::emitFunctionBodyStart() {
  MCInstLower.initialize(&MF->getContext());
  // TODO
}

void CDMAsmPrinter::emitFunctionBodyEnd() {
  // TODO
}

void CDMAsmPrinter::emitFunctionEntryLabel() {
  OutStreamer->emitLabel(CurrentFnSym);
}

void CDMAsmPrinter::emitLinkage(const GlobalValue *GV, MCSymbol *GVSym) const {
  // not needed (stub)
}

void CDMAsmPrinter::emitFunctionHeader() {
  // If something is missing, check original implementation
  // We don't want to emit anything here, but we want to preserve some of the
  // original functionality
  const Function &F = MF->getFunction();

  OutStreamer->getCommentOS()
      << "-- Begin function "
      << GlobalValue::dropLLVMManglingEscape(F.getName()) << '\n';

  // Print out constants referenced by the function
  emitConstantPool();

  if (MF->front().isBeginSection())
    MF->setSection(getObjFileLowering().getUniqueSectionForFunction(F, TM));
  else
    MF->setSection(getObjFileLowering().SectionForGlobal(&F, TM));
  OutStreamer->switchSection(MF->getSection());

  emitFunctionEntryLabel();
}

void CDMAsmPrinter::emitStartOfAsmFile(Module &Module) {
  collectAndEmitSourceFiles(Module);

  auto FN = Module.getSourceFileName();
  std::replace_if(
      FN.begin(), FN.end(), [](char C) { return !(isAlnum(C) || C == '_'); },
      '_');

  getTargetStreamer()->emitRsect(llvm::formatv("_{0}_{1}", FN, rand()));
}

void CDMAsmPrinter::emitEndOfAsmFile(Module &Module) {
  if (auto *TS = getTargetStreamer()) {
    TS->emitExtTable();
    TS->emitEnd();
  }
}