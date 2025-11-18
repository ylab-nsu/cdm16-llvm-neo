#include "CDMAsmStreamer.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSection.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

CDMAsmStreamer::CDMAsmStreamer(MCContext &Context,
                               std::unique_ptr<formatted_raw_ostream> os,
                               std::unique_ptr<MCInstPrinter> printer,
                               std::unique_ptr<MCCodeEmitter> emitter,
                               std::unique_ptr<MCAsmBackend> asmbackend)
    : MCStreamer(Context), OSOwner(std::move(os)), OS(*OSOwner),
      MAI(Context.getAsmInfo()), InstPrinter(std::move(printer)),
      CommentStream(CommentToEmit), IsVerboseAsm(true) {
  assert(InstPrinter && "CDMAsmStreamer created with no instruction printer");
}

void CDMAsmStreamer::emitLabel(MCSymbol *Symbol, SMLoc Loc) {
    MCStreamer::emitLabel(Symbol, Loc);

    Symbol->print(OS, MAI);
    OS << (Symbol->isExternal() ? ">" : ":");

    EmitEOL();
}

void CDMAsmStreamer::emitExplicitComments() {
    StringRef Comments = ExplicitCommentToEmit;
    if (!Comments.empty())
        OS << Comments;
    ExplicitCommentToEmit.clear();
}

void CDMAsmStreamer::EmitCommentsAndEOL() {
    if (CommentToEmit.empty() && CommentStream.GetNumBytesInBuffer() == 0) {
        OS << '\n';
        return;
    }
    StringRef Comments = CommentToEmit;

    assert(Comments.back() == '\n' &&
         "Comment array not newline terminated");
    do {
        OS.PadToColumn(MAI->getCommentColumn());
        size_t Position = Comments.find('\n');
        OS << MAI->getCommentString() << ' ' << Comments.substr(0, Position) <<'\n';

        Comments = Comments.substr(Position+1);
    } while (!Comments.empty());

    CommentToEmit.clear();
}

void CDMAsmStreamer::EmitEOL() {
    emitExplicitComments();
    if (!isVerboseAsm()) {
        OS << '\n';
        return;
    }
    EmitCommentsAndEOL();    
}

void CDMAsmStreamer::initSections(bool NoExecStack, const MCSubtargetInfo &STI) {
    AddComment("Init CDM sections");
    MCStreamer::initSections(NoExecStack, STI);
}

void CDMAsmStreamer::switchSectionNoPrint(MCSection *Section) {
    AddComment("switchSectionNoPrint CDM");
    MCStreamer::switchSectionNoPrint(Section);
}

void CDMAsmStreamer::switchSection(MCSection *Section, uint32_t Subsec) {
    AddComment("switchSection CDM");
    if (Section) {
        OS << "### SECTION: " << Section->getName() << "\n";
    }
    MCStreamer::switchSection(Section, Subsec);
}

void CDMAsmStreamer::AddComment(const Twine &T, bool EOL) {
    if (!IsVerboseAsm)
        return;
        
    CommentToEmit.clear();
    T.toVector(CommentToEmit);
    if (EOL && !CommentToEmit.empty() && CommentToEmit.back() != '\n')
        CommentToEmit.push_back('\n');
}

void CDMAsmStreamer::emitBytes(StringRef Data) {
    if (Data.empty())
        return;

    const char *Directive = MAI->getData8bitsDirective(); // \tdb\t
    for (const unsigned char C : Data.bytes()) {
        OS << Directive << (unsigned)C;
        EmitEOL();
    }
}

void CDMAsmStreamer::emitValueToAlignment(Align Alignment, int64_t Fill, uint8_t FillLen, unsigned MaxBytesToEmit) {
    if (Alignment == Align(1))
        return;
    
    OS << "\talign";
    if (Alignment.value() != 2) {
        OS << " " << Alignment.value();
    }
    EmitEOL();
}

void CDMAsmStreamer::emitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI) {
    MCStreamer::emitInstruction(Inst, STI);
    MCInstPrinter *InstPrinter = getInstPrinter();
    InstPrinter->printInst(&Inst, 0, "", STI, OS);
    EmitEOL();
}

void CDMAsmStreamer::emitIntValue(uint64_t Value, unsigned Size) { 
  emitValue(MCConstantExpr::create(Value, getContext()), Size);
}

void CDMAsmStreamer::emitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc) {
    const char *Directive = nullptr;
    switch (Size) {
        default: break;
        case 1: Directive = MAI->getData8bitsDirective();  break;
        case 2: Directive = MAI->getData16bitsDirective(); break;
        case 4: Directive = MAI->getData32bitsDirective(); break;
        case 8: Directive = MAI->getData64bitsDirective(); break;
    }
    if (Directive == nullptr) {
        report_fatal_error("Data directive for size " + Twine(Size) + " not defined");
    }
    OS << Directive;
    if (MCTargetStreamer *TS = getTargetStreamer()) {
        TS->emitValue(Value);
    } else {
        MAI->printExpr(OS, *Value);
    }
    EmitEOL();
    
    // Call base class to update internal state
    MCStreamer::emitValueImpl(Value, Size, Loc);
}

void CDMAsmStreamer::emitFill(const MCExpr &NumBytes, uint64_t FillValue, SMLoc Loc) {
    int64_t IntNumBytes;

    if (FillValue == 0) {
        OS << MAI->getZeroDirective(); // "\tds\t"
        MAI->printExpr(OS, NumBytes);
        EmitEOL();
        return;
    }
    
    if (!NumBytes.evaluateAsAbsolute(IntNumBytes)) 
        report_fatal_error("Cannot emit non-absolute expression lengths of fill.");

    if (IntNumBytes == 0)
        return;
    
    for (int i = 0; i < IntNumBytes; ++i) {
        OS << MAI->getData8bitsDirective() << (int)FillValue; // "\tdb\t"<<fill
        EmitEOL();
    }
}

void CDMAsmStreamer::emitFill(const MCExpr &NumValues, int64_t Size, int64_t Expr, SMLoc Loc) {
    int64_t IntNumValues;
    if (!NumValues.evaluateAsAbsolute(IntNumValues)) {
        report_fatal_error("Cannot emit non-absolute expression lengths of fill.");
    }
    
    if (IntNumValues == 0)
        return;
        
    const char *Directive = nullptr;
    switch (Size) {
        case 1: Directive = MAI->getData8bitsDirective(); break;
        case 2: Directive = MAI->getData16bitsDirective(); break;
        case 4: Directive = MAI->getData32bitsDirective(); break;
        case 8: Directive = MAI->getData64bitsDirective(); break;
        default: report_fatal_error("Invalid size for fill directive: " + Twine(Size));
    }
    
    for (int i = 0; i < IntNumValues; ++i) {
        OS << Directive << Expr;
        EmitEOL();
    }
}

void CDMAsmStreamer::emitRawTextImpl(StringRef String) {
    OS << String;
}