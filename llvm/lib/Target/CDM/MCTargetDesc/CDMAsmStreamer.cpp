#include "CDMAsmStreamer.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

CDMAsmStreamer::CDMAsmStreamer(MCContext &Context,
                               std::unique_ptr<formatted_raw_ostream> OS,
                               std::unique_ptr<MCInstPrinter> Printer,
                               std::unique_ptr<MCCodeEmitter> Emitter,
                               std::unique_ptr<MCAsmBackend> AsmBackend)
    : MCStreamer(Context), OSOwner(std::move(OS)), OS(*OSOwner),
      MAI(Context.getAsmInfo()), InstPrinter(std::move(Printer)),
      CommentStream(CommentToEmit) {
  assert(InstPrinter && "CDMAsmStreamer created with no instruction printer");
  Context.setUseNamesOnTempLabels(true);
  InstPrinter->setCommentStream(CommentStream);
  auto *TO = Context.getTargetOptions();
  if (TO) {
    IsVerboseAsm = TO->AsmVerbose;
  }
}

static void printSymbolName(StringRef Name, raw_ostream &OS) {
  // TODO This symbol name correction is temporary until we have
  // quoted symbol names in cocas.
  //
  // This can still produce invalid symbol names, because we don't
  // ensure that symbol names don't start with a dot or a digit or
  // end with a dot.
  for (char C : Name) {
    if (std::isalnum(C) || C == '_' || C == '.') {
      OS << C;
    } else {
      OS << "___";
      OS << llvm::format("%02X", (unsigned)C);
      OS << "___";
    }
  }
}

void CDMAsmStreamer::printExpr(raw_ostream &OS, const MCExpr &Expr) {
  switch (Expr.getKind()) {
  case MCExpr::SymbolRef: {
    const MCSymbolRefExpr &SRE = cast<MCSymbolRefExpr>(Expr);
    StringRef Name = SRE.getSymbol().getName();
    printSymbolName(Name, OS);
  } break;
  default:
    int64_t IntValue;
    if (!Expr.evaluateAsAbsolute(IntValue)) {
      report_fatal_error("Don't know how to emit this value.");
    }
    OS << IntValue;
    break;
  }
}

void CDMAsmStreamer::emitLabel(MCSymbol *Symbol, SMLoc Loc) {
  MCStreamer::emitLabel(Symbol, Loc);
  printSymbolName(Symbol->getName(), OS);
  OS << (Symbol->isExternal() ? ">" : ":");
  emitEOL();
}

void CDMAsmStreamer::visitUsedSymbol(const MCSymbol &Sym) {
  UsedSymbols.insert(&Sym);
}

static inline char toOctal(int X) { return (X & 7) + '0'; }

static void printQuotedString(StringRef Data, raw_ostream &OS) {
  OS << '"';
  for (unsigned char C : Data) {
    switch (C) {
    case '\\':
      OS << "\\\\";
      break;
    case '"':
      OS << "\\\"";
      break;
    case '\a':
      OS << "\\a";
      break;
    case '\b':
      OS << "\\b";
      break;
    case '\f':
      OS << "\\f";
      break;
    case '\n':
      OS << "\\n";
      break;
    case '\r':
      OS << "\\r";
      break;
    case '\t':
      OS << "\\t";
      break;
    case '\v':
      OS << "\\v";
      break;
    default:
      if (isPrint(C)) {
        OS << (char)C;
      } else {
        OS << '\\';
        OS << toOctal(C >> 6);
        OS << toOctal(C >> 3);
        OS << toOctal(C >> 0);
      }
    }
  }
  OS << '"';
}

static bool canPrintAsQuotedString(StringRef Data) {
  for (char C : Data) {
    // Cocas has UTF-8 strings so they cannot have arbitrary bytes
    if ((unsigned)C > 127) {
      return false;
    }
  }
  return true;
}

void CDMAsmStreamer::emitBytes(StringRef Data) {
  if (canPrintAsQuotedString(Data)) {
    OS << "\tdb\t";
    printQuotedString(Data, OS);
    emitEOL();
  } else {
    emitBinaryData(Data);
  }
}

void CDMAsmStreamer::emitBinaryData(StringRef Data) {
  const size_t Cols = 8;
  for (size_t I = 0, EI = alignTo(Data.size(), Cols); I < EI; I += Cols) {
    size_t J = I, EJ = std::min(I + Cols, Data.size());
    assert(EJ > 0);
    OS << "\tdb\t";
    for (; J < EJ - 1; ++J) {
      OS << format("0x%02x", uint8_t(Data[J])) << ", ";
    }
    OS << format("0x%02x", uint8_t(Data[J]));
    emitEOL();
  }
}

void CDMAsmStreamer::emitIntValue(uint64_t Value, unsigned Size) {
  emitValue(MCConstantExpr::create(Value, getContext()), Size);
}

void CDMAsmStreamer::emitValueImpl(const MCExpr *Value, unsigned Size,
                                   SMLoc Loc) {

  MCStreamer::emitValueImpl(Value, Size, Loc);

  const char *Directive;
  switch (Size) {
  case 1:
    Directive = "\tdb\t";
    break;
  case 2:
    Directive = "\tdc\t";
    break;
  default:
    Directive = nullptr;
    break;
  }

  if (Directive) {
    OS << Directive;
    SmallString<128> Str;
    raw_svector_ostream OS(Str);
    printExpr(OS, *Value);
    emitRawText(OS.str());
    return;
  }

  int64_t IntValue;
  if (!Value->evaluateAsAbsolute(IntValue)) {
    report_fatal_error("Don't know how to emit this value.");
  }
  SmallString<8> Str;
  for (unsigned BytesEmitted = 0; BytesEmitted < Size; BytesEmitted++) {
    Str.push_back((char)(IntValue >> (BytesEmitted * 8)));
  }
  emitBinaryData(Str);
}

void CDMAsmStreamer::emitFill(const MCExpr &NumBytes, uint64_t FillValue,
                              SMLoc Loc) {
  emitFill(NumBytes, 1, FillValue, Loc);
}

void CDMAsmStreamer::emitFill(const MCExpr &NumValues, int64_t Size,
                              int64_t Expr, SMLoc Loc) {
  int64_t IntNumValues;
  if (!NumValues.evaluateAsAbsolute(IntNumValues)) {
    report_fatal_error("Cannot emit non-absolute expression lengths of fill.");
  }

  if (IntNumValues == 0) {
    return;
  }

  if (Expr == 0) {
    int64_t IntNumBytes = IntNumValues * Size;
    OS << "\tds\t" << IntNumBytes;
    emitEOL();
    return;
  }

  for (int I = 0; I < IntNumValues; ++I) {
    emitIntValue(Expr, Size);
  }
}

void CDMAsmStreamer::emitValueToAlignment(Align Alignment, int64_t Fill,
                                          uint8_t FillLen,
                                          unsigned MaxBytesToEmit) {
  assert((Fill == 0) && "Non-zero fill value not supported for alignment");

  if (Alignment == Align(1)) {
    return;
  }
  OS << "\talign\t" << Alignment.value();
  emitEOL();
}

void CDMAsmStreamer::emitCodeAlignment(Align Alignment,
                                       const MCSubtargetInfo *STI,
                                       unsigned MaxBytesToEmit) {
  emitValueToAlignment(Alignment, 0, 1, MaxBytesToEmit);
}

void CDMAsmStreamer::emitInstruction(const MCInst &Inst,
                                     const MCSubtargetInfo &STI) {
  MCStreamer::emitInstruction(Inst, STI);
  MCInstPrinter *InstPrinter = getInstPrinter();
  InstPrinter->printInst(&Inst, 0, "", STI, OS);
  emitEOL();
}

void CDMAsmStreamer::switchSection(MCSection *Section, uint32_t Subsection) {
  if (getCurrentSection().first != Section ||
      getCurrentSection().second != Subsection) {
    emitRawText("### SECTION: " + Section->getName() + "\n");
  }
  MCStreamer::switchSection(Section, Subsection);
}

void CDMAsmStreamer::AddComment(const Twine &T, bool EOL) {
  if (!IsVerboseAsm) {
    return;
  }

  T.toVector(CommentToEmit);
  if (EOL && !CommentToEmit.empty() && CommentToEmit.back() != '\n') {
    CommentToEmit.push_back('\n');
  }
}

void CDMAsmStreamer::emitRawComment(const Twine &T, bool TabPrefix) {
  if (TabPrefix) {
    OS << '\t';
  }
  OS << '#' << T;
  emitEOL();
}

void CDMAsmStreamer::emitRawTextImpl(StringRef String) {
  String.consume_back("\n");
  OS << String;
  emitEOL();
}

void CDMAsmStreamer::emitEOL() {
  if (!isVerboseAsm()) {
    OS << '\n';
    return;
  }
  if (CommentToEmit.empty() && CommentStream.GetNumBytesInBuffer() == 0) {
    OS << '\n';
    return;
  }

  StringRef Comments = CommentToEmit;

  assert(Comments.back() == '\n' && "Comment array not newline terminated");
  do {
    OS.PadToColumn(48);
    size_t Position = Comments.find('\n');
    OS << "# " << Comments.substr(0, Position) << '\n';
    Comments = Comments.substr(Position + 1);
  } while (!Comments.empty());

  CommentToEmit.clear();
}

void CDMAsmStreamer::reset() {
  MCStreamer::reset();
  CommentToEmit.clear();
  UsedSymbols.clear();
}
