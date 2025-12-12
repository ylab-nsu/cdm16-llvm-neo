#ifndef LLVM_CDMASMSTREAMER_H
#define LLVM_CDMASMSTREAMER_H

#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"
#include <set>

namespace llvm {

class MCAsmInfo;
class MCInstPrinter;
class MCAssembler;
class MCCodeEmitter;
class MCAsmBackend;

class CDMAsmStreamer : public MCStreamer {
  std::string Str;
  std::unique_ptr<formatted_raw_ostream> OSOwner;
  formatted_raw_ostream &OS;
  const MCAsmInfo *MAI;
  std::unique_ptr<MCInstPrinter> InstPrinter;
  std::set<const MCSymbol *> UsedSymbols;
  SmallString<128> CommentToEmit;
  raw_svector_ostream CommentStream;
  raw_null_ostream NullStream;
  bool IsVerboseAsm;

  bool EmittedSectionDirective = false;

protected:
  void emitRawTextImpl(StringRef String) override;

public:
  CDMAsmStreamer(MCContext &Context, std::unique_ptr<formatted_raw_ostream> OS,
                 std::unique_ptr<MCInstPrinter> Printer,
                 std::unique_ptr<MCCodeEmitter> Emitter,
                 std::unique_ptr<MCAsmBackend> AsmBackend);

  // This should be used instead of MCAsmInfo::printExpr
  // to make sure that we don't print invalid expressions.
  static void printExpr(raw_ostream &OS, const MCExpr &Expr);

  // CDM-specific functions
  void emitRsect(const Twine &Name);
  void emitDbgSource(unsigned FileIndex, const Twine &FileName);
  void emitDbgLoc(unsigned Index, unsigned Line, unsigned Column);
  void emitExtList();
  void emitEnd();

  MCInstPrinter *getInstPrinter() { return InstPrinter.get(); }
  MCAssembler *getAssemblerPtr() override { return nullptr; }

  raw_ostream &getCommentOS() override {
    if (!IsVerboseAsm) {
      return nulls();
    }
    return CommentStream;
  }

  bool emitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute) override {
    return false;
  }

  void emitCommonSymbol(MCSymbol *Symbol, uint64_t Size,
                        Align ByteAlignment) override {
    // CDM doesn't support common symbols
  }

  bool isVerboseAsm() const override { return IsVerboseAsm; }

  bool hasRawTextSupport() const override { return true; }

  void emitLabel(MCSymbol *Symbol, SMLoc Loc) override;

  void visitUsedSymbol(const MCSymbol &Sym) override;

  void emitEOL();

  void addBlankLine() override { emitEOL(); }

  void switchSection(MCSection *Section, uint32_t Subsection) override;

  void emitBytes(StringRef Data) override;

  void emitBinaryData(StringRef Data) override;

  void emitValueToAlignment(Align Alignment, int64_t Fill, uint8_t FillLen,
                            unsigned MaxBytesToEmit) override;

  void emitCodeAlignment(Align Alignment, const MCSubtargetInfo *STI,
                         unsigned MaxBytesToEmit = 0) override;

  void emitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI) override;

  void emitIntValue(uint64_t Value, unsigned Size) override;

  void emitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc) override;

  void AddComment(const Twine &T, bool EOL = true) override;

  void emitRawComment(const Twine &T, bool TabPrefix = true) override;

  void emitFill(const MCExpr &NumBytes, uint64_t FillValue,
                SMLoc Loc = SMLoc()) override;
  void emitFill(const MCExpr &NumValues, int64_t Size, int64_t Expr,
                SMLoc Loc = SMLoc()) override;

  void reset() override;

  static bool classof(const MCStreamer *S) { return S->hasRawTextSupport(); }
};

} // namespace llvm

#endif // LLVM_CDMASMSTREAMER_H
