//
// Created by ilya on 16.10.23.
//

#include "CDMAsmPrinter.h"
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

          if (!getSourceFileIndex(Checksum)) {
            OutStreamer->emitRawText(formatv("dbg_source {0}, \"{1}\"\n",
                                             this->SourceFiles.size(), Path));
            this->SourceFiles.insert({Checksum, this->SourceFiles.size()});
          }
        }
      }
    }
  }

  OutStreamer->emitRawText("\n\n");
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
  //  OutStreamer->emitRawText(llvm::formatv("{0}>", CurrentFnSym->getName()));
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

  OutStreamer->emitRawText("memset, memcpy, memmove, __mulhi3, __divhi3, __udivhi3, __modhi3, __umodhi3, __mulsi3, __divsi3, __udivsi3, __modsi3, __umodsi3, __muldi3, __divdi3, __udivdi3, __moddi3, __umoddi3: ext\n");

  auto FN = Module.getSourceFileName();

  std::replace_if(
      FN.begin(), FN.end(), [](char C) { return !(isAlnum(C) || C == '_'); },
      '_');
  OutStreamer->emitRawText(llvm::formatv("rsect _{0}_{1}\n\n", FN, rand()));

  std::set<std::string> PrefixesToIgnore = {"llvm.lifetime.", "llvm."};

  for (auto &GV : Module.global_objects()) {
    if (GV.isDeclaration() and
        std::find_if(PrefixesToIgnore.begin(), PrefixesToIgnore.end(),
                     [&](auto Pref) {
                       return GV.getName().starts_with(Pref);
                     }) == PrefixesToIgnore.end()) {
      OutStreamer->emitRawText(llvm::formatv("{0}: ext\n", GV.getName()));
    }
  }

  // for (auto &ExternalSymbolName : ExternalSymbolNames) {
  //   OutStreamer->emitRawText(formatv("{0}: ext\n", ExternalSymbolName));
  // }

}

void CDMAsmPrinter::emitEndOfAsmFile(Module &Module) {
  OutStreamer->emitRawText("end.");
}

// TODO: implement target streamer
CDMAsmTargetStreamer::CDMAsmTargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S) {}
void CDMAsmTargetStreamer::emitLabel(MCSymbol *Symbol) {}

void CDMAsmTargetStreamer::changeSection(const MCSection *CurSection,
                                         MCSection *Section,
                                         uint32_t SubSection,
                                         raw_ostream &OS) {
  // This is a stub. We don't have sections in cdm
  // Section->
  // OS << llvm::formatv("rsect[{0}] ", Section->getName()/*Without first
  // dot*/);
  OS << llvm::formatv("### SECTION: {0}\n", Section->getName());
}
