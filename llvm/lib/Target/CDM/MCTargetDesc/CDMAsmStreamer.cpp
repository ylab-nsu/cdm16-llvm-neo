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
    Context.setUseNamesOnTempLabels(true);

    auto *TO = Context.getTargetOptions();
    if (!TO) {
        return;
    }
    IsVerboseAsm = TO->AsmVerbose;
    if (IsVerboseAsm) {
        InstPrinter->setCommentStream(CommentStream);
    }
}

void CDMAsmStreamer::emitLabel(MCSymbol *Symbol, SMLoc Loc) {
    MCStreamer::emitLabel(Symbol, Loc);

    SmallString<128> Str;
    raw_svector_ostream iOS(Str);

    Symbol->print(iOS, MAI);

    for (char C : Str) {
        if (std::isalnum(C) || C == '_') {
            OS << C;
        } else {
            OS << "___";
            OS << llvm::format("%02X", static_cast<unsigned char>(C));
            OS << "___";
        }
    }

    OS << (Symbol->isExternal() ? ">" : ":");

    emitEOL();
}

void CDMAsmStreamer::emitExplicitComments() {
    StringRef Comments = ExplicitCommentToEmit;
    if (!Comments.empty()) {
        OS << Comments;
    }
    ExplicitCommentToEmit.clear();
}

void CDMAsmStreamer::emitEOL() {
    emitExplicitComments();

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
        OS.PadToColumn(MAI->getCommentColumn());
        size_t Position = Comments.find('\n');
        OS << MAI->getCommentString() << ' ' << Comments.substr(0, Position) << '\n';

        Comments = Comments.substr(Position+1);
    } while (!Comments.empty());

    CommentToEmit.clear();
}

void CDMAsmStreamer::switchSection(MCSection *Section, uint32_t Subsection) {
    AddComment("switchSection CDM");
    if (getCurrentSection().first != Section || getCurrentSection().second != Subsection) {
        Section->printSwitchToSection(*MAI, getContext().getTargetTriple(), OS, Subsection);
    }
    MCStreamer::switchSection(Section, Subsection);
}

void CDMAsmStreamer::AddComment(const Twine &T, bool EOL) {
    if (!IsVerboseAsm) {
        return;
    }
        
    CommentToEmit.clear();
    T.toVector(CommentToEmit);
    if (EOL && !CommentToEmit.empty() && CommentToEmit.back() != '\n')
        CommentToEmit.push_back('\n');
}

void CDMAsmStreamer::emitBytes(StringRef Data) {
    if (Data.empty()) {
        return;
    }

    const char *Directive = "\tdb\t";
    for (const unsigned char C : Data.bytes()) {
        OS << Directive << (unsigned)C;
        emitEOL();
    }
}

void CDMAsmStreamer::emitValueToAlignment(Align Alignment, int64_t Fill, uint8_t FillLen, unsigned MaxBytesToEmit) {
    assert((Fill == 0) && "Non-zero fill value not supported for alignment");

    if (Alignment == Align(1)) {
        return;
    }

    OS << "\talign" << " " << Alignment.value();
    emitEOL();
}

void CDMAsmStreamer::emitCodeAlignment(Align Alignment, const MCSubtargetInfo *STI, unsigned MaxBytesToEmit) {
    emitValueToAlignment(Alignment, 0, 1, MaxBytesToEmit);
}

void CDMAsmStreamer::emitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI) {
    MCStreamer::emitInstruction(Inst, STI);
    MCInstPrinter *InstPrinter = getInstPrinter();
    InstPrinter->printInst(&Inst, 0, "", STI, OS);
    emitEOL();
}

void CDMAsmStreamer::emitIntValue(uint64_t Value, unsigned Size) { 
  emitValue(MCConstantExpr::create(Value, getContext()), Size);
}

void CDMAsmStreamer::emitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc) {
    SmallString<128> Str;
    raw_svector_ostream iOS(Str);
    const char *Directive = nullptr;

    switch (Size) {
        case 1: Directive = "\tdb\t";  break;
        case 2: Directive = "\tdc\t"; break;
        default: Directive = nullptr; break;
    }

    if (Directive) {
        OS << Directive;
        getContext().getAsmInfo()->printExpr(iOS, *Value);
        emitRawText(iOS.str());

        MCStreamer::emitValueImpl(Value, Size, Loc);
        return;
    }

    int64_t IntValue;
    if (!Value->evaluateAsAbsolute(IntValue)) {
        report_fatal_error("Don't know how to emit this value.");
    }

    for (unsigned Emitted = 0; Emitted != Size;) {
        unsigned Remaining = Size - Emitted;
        unsigned EmissionSize = llvm::bit_floor(std::min(Remaining, (unsigned)Size - 1));

        unsigned ByteOffset = Emitted;
        uint64_t ValueToEmit = IntValue >> (ByteOffset * 8);

        uint64_t Shift = 64 - EmissionSize * 8;
        ValueToEmit &= ~0ULL >> Shift;
        emitIntValue(ValueToEmit, EmissionSize);
        Emitted += EmissionSize;
    }
}

void CDMAsmStreamer::emitFill(const MCExpr &NumBytes, uint64_t FillValue, SMLoc Loc) {
    int64_t IntNumBytes;

    if (FillValue == 0) {
        OS << "\tds\t";
        getContext().getAsmInfo()->printExpr(OS, NumBytes);
        emitEOL();
        return;
    }
    
    if (!NumBytes.evaluateAsAbsolute(IntNumBytes)) {
        report_fatal_error("Cannot emit non-absolute expression lengths of fill.");
    }
    
    if (IntNumBytes == 0) {
        return;
    }

    for (int i = 0; i < IntNumBytes; ++i) {
        OS << "\tdb\t" << (int)FillValue;
        emitEOL();
    }
}

void CDMAsmStreamer::emitFill(const MCExpr &NumValues, int64_t Size, int64_t Expr, SMLoc Loc) {
    int64_t IntNumValues;
    if (!NumValues.evaluateAsAbsolute(IntNumValues)) {
        report_fatal_error("Cannot emit non-absolute expression lengths of fill.");
    }
    
    if (IntNumValues == 0) {
        return;
    }
        
    const char *Directive = nullptr;
    switch (Size) {
        case 1: Directive = "\tdb\t"; break;
        case 2: Directive = "\tdc\t"; break;
        default: Directive = nullptr; break;
    }
    
    if (Directive) {
        for (int i = 0; i < IntNumValues; ++i) {
            OS << Directive << Expr;
            emitEOL();
        }

        return;
    }

    for (unsigned Emitted = 0; Emitted != Size;) {
        unsigned Remaining = Size - Emitted;
        unsigned EmissionSize = llvm::bit_floor(std::min(Remaining, (unsigned)Size - 1));

        unsigned ByteOffset = Emitted;
        uint64_t ValueToEmit = (uint64_t)Expr >> (ByteOffset * 8);

        uint64_t Shift = 64 - EmissionSize * 8;
        ValueToEmit &= ~0ULL >> Shift;
        emitIntValue(ValueToEmit, EmissionSize);
        Emitted += EmissionSize;
    }
}

void CDMAsmStreamer::emitRawTextImpl(StringRef String) {
    OS << String;
    emitEOL();
}