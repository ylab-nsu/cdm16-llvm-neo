//
// Created by ilya on 16.10.23.
//

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

#include "CDM.h"
#include "CDMMCInstLower.h"
#include "InstPrinter/CDMInstPrinter.h"
#include "MCTargetDesc/CDMTargetStreamer.h"
#include "TargetInfo/CDMTargetInfo.h"

#define DEBUG_TYPE "cdm-asm-printer"
#define PASS_NAME "CDM Assembly Printer"

using namespace llvm;

namespace {

class CDMAsmPrinter : public AsmPrinter {
  llvm::StringMap<int> SourceFiles;
  std::optional<unsigned> SourceFileIndex;
  std::optional<unsigned> LineNumber;
  std::optional<unsigned> ColumnNumber;

  std::optional<int> getSourceFileIndex(StringRef Checksum);
  void collectAndEmitSourceFiles(Module &Module);
  void emitDebugLoc(DILocation &DebugLoc);

public:
  explicit CDMAsmPrinter(TargetMachine &TM,
                         std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return PASS_NAME; }

  void emitInstruction(const MachineInstr *Instr) override;
  void emitStartOfAsmFile(Module &Module) override;

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                       const char *ExtraCode, raw_ostream &O) override;
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                             const char *ExtraCode, raw_ostream &OS) override;

  CDMTargetStreamer *getTargetStreamer() const {
    return static_cast<CDMTargetStreamer *>(OutStreamer->getTargetStreamer());
  }

  static char ID;
};

} // namespace

std::optional<int> CDMAsmPrinter::getSourceFileIndex(StringRef Checksum) {
  if (this->SourceFiles.contains(Checksum)) {
    return this->SourceFiles.lookup(Checksum);
  }

  return std::nullopt;
}

void CDMAsmPrinter::collectAndEmitSourceFiles(Module &Module) {
  bool Emitted = false;

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
            getTargetStreamer()->emitDbgSource(this->SourceFiles.size(), Path);
            this->SourceFiles.insert({Checksum, this->SourceFiles.size()});
            Emitted = true;
          }
        }
      }
    }
  }

  if (Emitted) {
    OutStreamer->addBlankLine();
  }
}

void CDMAsmPrinter::emitDebugLoc(DILocation &DebugLoc) {
  StringRef Checksum = DebugLoc.getFile()->getChecksum().value().Value;

  std::optional<int> NewFileIndex = getSourceFileIndex(Checksum);
  if (!NewFileIndex) {
    return;
  }

  unsigned NewLineNumber = DebugLoc.getLine();
  unsigned NewColumnNumber = DebugLoc.getColumn();

  if (SourceFileIndex != NewFileIndex || LineNumber != NewLineNumber ||
      ColumnNumber != NewColumnNumber) {
    SourceFileIndex = NewFileIndex;
    LineNumber = NewLineNumber;
    ColumnNumber = NewColumnNumber;

    getTargetStreamer()->emitDbgLoc(*SourceFileIndex, *LineNumber,
                                    *ColumnNumber, DebugLoc.getFilename());
  }
}

void CDMAsmPrinter::emitInstruction(const MachineInstr *Instr) {
  if (Instr->isDebugValue()) {
    // TODO: implement
    return;
  }
  if (Instr->isDebugLabel()) {
    return;
  }

  DILocation *DebugLoc = Instr->getDebugLoc().get();
  if (DebugLoc) {
    emitDebugLoc(*DebugLoc);
  }

  CDMMCInstLower MCInstLower(OutContext, *this);

  MachineBasicBlock::const_instr_iterator I = Instr->getIterator();
  MachineBasicBlock::const_instr_iterator E = Instr->getParent()->instr_end();

  // If instruction we emit is actually inside an instruction bundle,
  // iterate over all instructions in bundle and emit them all
  do {
    // Skip bundle pseudo instruction and emit content of a bundle
    if (I->isBundle()) {
      continue;
    }

    if (I->isPseudo())
      llvm_unreachable("Pseudo opcode found in emitInstruction()");

    MCInst TmpInst0;
    MCInstLower.lower(&*I, TmpInst0);
    OutStreamer->emitInstruction(TmpInst0, getSubtargetInfo());
  } while ((++I != E) && I->isInsideBundle());
}

void CDMAsmPrinter::emitStartOfAsmFile(Module &Module) {
  collectAndEmitSourceFiles(Module);

  auto FN = Module.getSourceFileName();
  std::replace_if(
      FN.begin(), FN.end(), [](char C) { return !(isAlnum(C) || C == '_'); },
      '_');

  getTargetStreamer()->emitRsect(llvm::formatv("_{0}_{1}", FN, rand()));
}

bool CDMAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                    const char *ExtraCode, raw_ostream &OS) {
  if (ExtraCode && ExtraCode[0]) {
    return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, OS);
  }
  const MachineOperand &MO = MI->getOperand(OpNo);
  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    OS << CDMInstPrinter::getRegisterName(MO.getReg());
    break;
  case MachineOperand::MO_Immediate:
    OS << MO.getImm();
    break;
  case MachineOperand::MO_MachineBasicBlock:
    MO.getMBB()->getSymbol()->print(OS, MAI);
    break;
  case MachineOperand::MO_GlobalAddress:
    PrintSymbolOperand(MO, OS);
    break;
  case MachineOperand::MO_ExternalSymbol:
    OS << *GetExternalSymbolSymbol(MO.getSymbolName());
    break;
  case MachineOperand::MO_BlockAddress:
    GetBlockAddressSymbol(MO.getBlockAddress())->print(OS, MAI);
    break;
  case MachineOperand::MO_ConstantPoolIndex: {
    const DataLayout &DL = getDataLayout();
    OS << DL.getPrivateGlobalPrefix() << "CPI" << getFunctionNumber() << '_'
       << MO.getIndex();
    break;
  }
  default:
    return true;
  }
  return false;
}

bool CDMAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                                          const char *ExtraCode,
                                          raw_ostream &OS) {
  if (ExtraCode) {
    return AsmPrinter::PrintAsmMemoryOperand(MI, OpNo, ExtraCode, OS);
  }

  const MachineOperand &Reg = MI->getOperand(OpNo);

  // All memory operands should be a register.
  if (!Reg.isReg())
    return true;

  OS << CDMInstPrinter::getRegisterName(Reg.getReg());
  return false;
}

char CDMAsmPrinter::ID = 0;

INITIALIZE_PASS(CDMAsmPrinter, DEBUG_TYPE, PASS_NAME, false, false)

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeCDMAsmPrinter() {
  llvm::RegisterAsmPrinter<CDMAsmPrinter> X(getTheCDMTarget());
}
