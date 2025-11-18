#ifndef LLVM_CDMASMSTREAMER_H
#define LLVM_CDMASMSTREAMER_H

#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"

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
  SmallString<128> ExplicitCommentToEmit;
  SmallString<128> CommentToEmit;
  raw_svector_ostream CommentStream;
  raw_null_ostream NullStream;
  bool IsVerboseAsm;

protected:
  void emitRawTextImpl(StringRef String) override;

public:
  CDMAsmStreamer(MCContext &Context,
                 std::unique_ptr<formatted_raw_ostream> os,
                 std::unique_ptr<MCInstPrinter> printer,
                 std::unique_ptr<MCCodeEmitter> emitter,
                 std::unique_ptr<MCAsmBackend> asmbackend);

  MCInstPrinter *getInstPrinter() { return InstPrinter.get(); }
  MCAssembler *getAssemblerPtr() override { return nullptr; }

  bool emitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute) override {
    return false;
  }

  void emitCommonSymbol(MCSymbol *Symbol, uint64_t Size, Align ByteAlignment) override {
    // CDM doesn't support common symbols
  }

  bool isVerboseAsm() const override { return IsVerboseAsm; }

  bool hasRawTextSupport() const override { return true; }

  void emitLabel(MCSymbol *Symbol, SMLoc Loc) override;

  void EmitEOL();
  void EmitCommentsAndEOL();

  void emitExplicitComments() override;

  void initSections(bool NoExecStack, const MCSubtargetInfo &STI) override;
  virtual void switchSectionNoPrint(MCSection *Section) override;
  virtual void switchSection(MCSection *Section, uint32_t Subsec = 0) override;

  void emitBytes(StringRef Data) override;

  void emitValueToAlignment(Align Alignment, int64_t Fill, uint8_t FillLen,
                            unsigned MaxBytesToEmit) override;

  void emitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI) override;

  void emitIntValue(uint64_t Value, unsigned Size) override;
  
  void emitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc) override;

  void AddComment(const Twine &T, bool EOL = true) override;
  void emitFill(const MCExpr &NumBytes, uint64_t FillValue,
                SMLoc Loc = SMLoc()) override;
  void emitFill(const MCExpr &NumValues, int64_t Size, int64_t Expr,
                SMLoc Loc = SMLoc()) override;
};

} // namespace llvm

#endif // LLVM_CDMASMSTREAMER_H