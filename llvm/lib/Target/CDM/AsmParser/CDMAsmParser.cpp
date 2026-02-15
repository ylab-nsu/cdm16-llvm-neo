#include "CDMInstrInfo.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "TargetInfo/CDMTargetInfo.h"

#include "llvm/MC/MCAsmMacro.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"

#include "llvm/Support/MathExtras.h"
#define DEBUG_TYPE "cdm-asm-parser"

using namespace llvm;

namespace {

/// Parses CDM assembly from a stream.
class CDMAsmParser : public MCTargetAsmParser {
  const MCSubtargetInfo &STI;
  MCAsmParser &Parser;
  const MCRegisterInfo *MRI;

#define GET_ASSEMBLER_HEADER
#include "CDMGenAsmMatcher.inc"

  bool invalidOperandError(SMLoc Loc, OperandVector &Operands,
                           uint64_t ErrorInfo);
  bool outOfRangeError(OperandVector &Operands, uint64_t ErrorInfo,
                       int64_t Lower, int64_t Upper, StringRef Msg);

  /// Post-processes and emits instruction
  bool emit(MCInst &Inst, SMLoc const &Loc, MCStreamer &Out) const;

  /// Matches a register name to an MCRegister
  MCRegister getRegisterByName(StringRef Name);

  /// Checks if an instruction mnemonic is valid
  bool mnemonicIsValid(StringRef Mnemonic, unsigned VariantID);

  /// Parses a branch instruction mnemonic
  bool parseBranchInst(StringRef &Name, SMLoc Loc, OperandVector &Operands);

  /// Parses a generic immediate value.
  ParseStatus parseImm(OperandVector &Operands);

  /// Parses an immediate value that has be known at assembly time.
  ParseStatus parseSmallImm(OperandVector &Operands);

public:
  enum CDMMatchResultTy : unsigned {
    Match_Dummy = FIRST_TARGET_MATCH_RESULT_TY,
#define GET_OPERAND_DIAGNOSTIC_TYPES
#include "CDMGenAsmMatcher.inc"
#undef GET_OPERAND_DIAGNOSTIC_TYPES
  };

  CDMAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
               const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII), STI(STI), Parser(Parser) {
    MCAsmParserExtension::Initialize(Parser);
    MRI = getContext().getRegisterInfo();

    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }

  bool parseRegister(MCRegister &Reg, SMLoc &StartLoc, SMLoc &EndLoc) override;
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;

  bool parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;
};

/// A parsed CDM assembly operand.
class CDMOperand : public MCParsedAsmOperand {
  typedef MCParsedAsmOperand Base;

  enum class KindTy {
    Invalid,
    Token,
    Reg,
    Cond,
    Imm,
  };

  KindTy Kind;
  SMLoc Start, End;
  union {
    StringRef Token;
    MCRegister Reg;
    CDMCOND::CondOp Cond;
    const MCExpr *Expr;
  };

public:
  CDMOperand(KindTy Kind, SMLoc Start, SMLoc End)
      : Base(), Kind(Kind), Start(Start), End(End) {}

  SMLoc getStartLoc() const override { return Start; }
  SMLoc getEndLoc() const override { return End; }
  bool isMem() const override { return false; }

  void print(raw_ostream &OS, const MCAsmInfo &MAI) const override;

  // Token (instruction mnemonic)
  bool isToken() const override;
  StringRef getToken() const;
  static std::unique_ptr<CDMOperand> createToken(StringRef Token, SMLoc Start,
                                                 SMLoc End);

  // Register operand
  bool isReg() const override;
  MCRegister getReg() const override;
  void addRegOperands(MCInst &Inst, unsigned N) const;
  static std::unique_ptr<CDMOperand> createReg(MCRegister Reg, SMLoc Start,
                                               SMLoc End);

  // Condition operand
  bool isCond() const;
  CDMCOND::CondOp getCond() const;
  void addCondOperands(MCInst &Inst, unsigned N) const;
  static std::unique_ptr<CDMOperand> createCond(CDMCOND::CondOp Reg,
                                                SMLoc Start, SMLoc End);

  // Immediate operand
  bool isImm() const override;
  void addImmOperands(MCInst &Inst, unsigned N) const;
  static std::unique_ptr<CDMOperand> createImm(const MCExpr *Expr, SMLoc Start,
                                               SMLoc End);

  /// Checks if an immediate operand is valid.
  ///
  /// @tparam Bits The bit-width of the field the value needs to fit in
  /// @tparam ShiftAmount The number of bits the value is shifted by in the
  /// encoding
  /// @tparam Signed Can the value be interpreted as signed
  /// @tparam Unsigned Can the value be interpreted as unsigned
  /// @tparam AllowSymbols Can the value be not known at assembly time (i.e.
  /// contain symbols)
  template <unsigned Bits, unsigned ShiftAmount, bool Signed, bool Unsigned,
            bool AllowSymbols = false>
  bool isImmN() const;

  /// Checks if an immediate operand is a valid shift amount.
  bool isShamt() const;
};

} // end anonymous namespace.

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCDMAsmParser() {
  RegisterMCAsmParser<CDMAsmParser> X(getTheCDMTarget());
}

/// Maps from the set of all register names to a register number.
/// \note Generated by TableGen.
static MCRegister MatchRegisterName(StringRef Name);

/// Maps from the set of all alternative registernames to a register number.
/// \note Generated by TableGen.
static MCRegister MatchRegisterAltName(StringRef Name);

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "CDMGenAsmMatcher.inc"

// Token (instruction mnemonic)
bool CDMOperand::isToken() const { return Kind == KindTy::Token; }
StringRef CDMOperand::getToken() const {
  assert(isToken());
  return Token;
}

std::unique_ptr<CDMOperand> CDMOperand::createToken(StringRef Token,
                                                    SMLoc Start, SMLoc End) {
  auto Op = std::make_unique<CDMOperand>(KindTy::Token, Start, End);
  Op->Token = Token;
  return Op;
}

// Register operand
bool CDMOperand::isReg() const { return Kind == KindTy::Reg; }

MCRegister CDMOperand::getReg() const {
  assert(isReg());
  return Reg;
}

void CDMOperand::addRegOperands(MCInst &Inst, unsigned N) const {
  assert(isReg() && "wrong operand kind");
  assert((N == 1) && "can only handle one register operand");

  Inst.addOperand(MCOperand::createReg(getReg()));
}

std::unique_ptr<CDMOperand> CDMOperand::createReg(MCRegister Reg, SMLoc Start,
                                                  SMLoc End) {
  auto Op = std::make_unique<CDMOperand>(KindTy::Reg, Start, End);
  Op->Reg = Reg;
  return Op;
}

bool CDMOperand::isCond() const { return Kind == KindTy::Cond; }

CDMCOND::CondOp CDMOperand::getCond() const {
  assert(isCond());
  return Cond;
}

void CDMOperand::addCondOperands(MCInst &Inst, unsigned N) const {
  assert(isCond() && "wrong operand kind");
  assert((N == 1) && "can only handle one condition operand");

  Inst.addOperand(MCOperand::createImm(getCond()));
}

std::unique_ptr<CDMOperand> CDMOperand::createCond(CDMCOND::CondOp Cond,
                                                   SMLoc Start, SMLoc End) {
  auto Op = std::make_unique<CDMOperand>(KindTy::Cond, Start, End);
  Op->Cond = Cond;
  return Op;
}

// Immmediate operand
bool CDMOperand::isImm() const {
  if (Kind != KindTy::Imm) {
    return false;
  }
  return true;
}

void CDMOperand::addImmOperands(MCInst &Inst, unsigned N) const {
  assert(isImm() && "wrong operand kind");
  assert((N == 1) && "can only handle one immediate operand");

  if (const auto *Const = dyn_cast<MCConstantExpr>(Expr)) {
    Inst.addOperand(MCOperand::createImm(Const->getValue()));
    return;
  }

  Inst.addOperand(MCOperand::createExpr(Expr));
}

std::unique_ptr<CDMOperand> CDMOperand::createImm(const MCExpr *Expr,
                                                  SMLoc Start, SMLoc End) {
  auto Op = std::make_unique<CDMOperand>(KindTy::Imm, Start, End);
  Op->Expr = Expr;
  return Op;
}

template <unsigned Bits, unsigned ShiftAmount, bool Signed, bool Unsigned,
          bool AllowSymbols>
bool CDMOperand::isImmN() const {
  if (!isImm()) {
    return false;
  }
  int64_t Value;
  if (!Expr->evaluateAsAbsolute(Value)) {
    return AllowSymbols;
  }
  return (Signed && isShiftedInt<Bits, ShiftAmount>(Value)) ||
         (Unsigned && isShiftedUInt<Bits, ShiftAmount>(Value));
}

bool CDMOperand::isShamt() const {
  if (!isImm()) {
    return false;
  }
  int64_t Value;
  if (!Expr->evaluateAsAbsolute(Value)) {
    return false;
  }
  return isUInt<3>(Value - 1);
}

void CDMOperand::print(raw_ostream &OS, const MCAsmInfo &MAI) const {
  switch (Kind) {
  case KindTy::Invalid:
    OS << "invalid";
    break;

  case KindTy::Reg:
    OS << "reg " << Reg;
    break;

  case KindTy::Token:
    OS << "token '" << Token << "'";
    break;

  case KindTy::Imm: {
    int64_t Value;
    Expr->evaluateAsAbsolute(Value);
    OS << "immediate " << Value;
    break;
  }
  case KindTy::Cond: {
    OS << "cond " << CDMCOND::condtoString(Cond);
    break;
  }
  }
}

ParseStatus CDMAsmParser::parseImm(OperandVector &Operands) {
  SMLoc Start = getLexer().getLoc();

  SMLoc End;
  const MCExpr *Expr;

  if (getParser().parseExpression(Expr, End))
    return ParseStatus::Failure;

  Operands.push_back(CDMOperand::createImm(Expr, Start, End));
  return ParseStatus::Success;
}

ParseStatus CDMAsmParser::parseSmallImm(OperandVector &Operands) {
  AsmToken Current = getParser().getTok();
  // Our syntax is ambiguous:
  // push fp
  // - does it push register fp to the stack?
  // - does it push the value of symbol fp to the stack?
  // So we just prohibit immediates starting with a register name.
  if (Current.is(AsmToken::Identifier) &&
      getRegisterByName(Current.getString())) {
    return ParseStatus::NoMatch;
  }

  SMLoc Start = getLexer().getLoc();

  SMLoc End;
  const MCExpr *Expr;

  if (getParser().parseExpression(Expr, End))
    return ParseStatus::Failure;

  Operands.push_back(CDMOperand::createImm(Expr, Start, End));
  return ParseStatus::Success;
}

MCRegister CDMAsmParser::getRegisterByName(StringRef Name) {
  MCRegister Reg;
  auto RegisterNameLower = Name.lower();
  Reg = MatchRegisterName(RegisterNameLower);
  if (!Reg) {
    Reg = MatchRegisterAltName(RegisterNameLower);
  }
  return Reg;
}

bool CDMAsmParser::parseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                 SMLoc &EndLoc) {
  ParseStatus Result = tryParseRegister(Reg, StartLoc, EndLoc);
  if (!Result.isSuccess())
    return Error(StartLoc, "expected register");

  return false;
}

ParseStatus CDMAsmParser::tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                           SMLoc &EndLoc) {
  LLVM_DEBUG(dbgs() << "tryParseRegister "; getTok().dump(dbgs());
             dbgs() << "\n");
  StartLoc = getLexer().getLoc();

  if (!Parser.getTok().is(AsmToken::Identifier)) {
    return ParseStatus::NoMatch;
  }

  Reg = getRegisterByName(Parser.getTok().getString());

  ParseStatus Result;
  if (!Reg) {
    Result = ParseStatus::NoMatch;
  } else {
    Parser.Lex();
    Result = ParseStatus::Success;
  }

  EndLoc = getLexer().getLoc();
  return Result;
}

bool CDMAsmParser::parseBranchInst(StringRef &Name, SMLoc Loc,
                                   OperandVector &Operands) {
  if (Name.size() < 2 || std::tolower(Name[0]) != 'b') {
    return true;
  }
  StringRef CondName = StringRef(Name.data() + 1, Name.size() - 1);
  CDMCOND::CondOp Cond = CDMCOND::stringToCond(CondName);
  if (Cond == CDMCOND::Invalid) {
    return true;
  }
  Name = StringRef(Name.data(), 1);
  SMLoc CondLoc = SMLoc::getFromPointer(Loc.getPointer() + 1);
  Operands.push_back(CDMOperand::createToken(Name, Loc, Loc));
  Operands.push_back(CDMOperand::createCond(Cond, CondLoc, CondLoc));
  return false;
}

bool CDMAsmParser::mnemonicIsValid(StringRef Mnemonic, unsigned VariantID) {
  // Process all MnemonicAliases to remap the mnemonic.
  applyMnemonicAliases(Mnemonic, getAvailableFeatures(), VariantID);

  // Find the appropriate table for this asm variant.
  const MatchEntry *Start, *End;
  switch (VariantID) {
  default:
    llvm_unreachable("invalid variant!");
  case 0:
    Start = std::begin(MatchTable0);
    End = std::end(MatchTable0);
    break;
  }

  // Search the table.
  auto MnemonicRange = std::equal_range(Start, End, Mnemonic, LessOpcode());
  return MnemonicRange.first != MnemonicRange.second;
}

bool CDMAsmParser::parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                    SMLoc NameLoc, OperandVector &Operands) {
  if (parseBranchInst(Name, NameLoc, Operands)) {
    Operands.push_back(CDMOperand::createToken(Name, NameLoc, NameLoc));
  }

  // Validate instruction mnemonic early so that we don't fail trying to parse a
  // register operand producing a confusing error message
  if (!mnemonicIsValid(Name, 0)) {
    return Error(NameLoc, "invalid instruction mnemonic");
  }

  bool First = true;
  while (Parser.getTok().isNot(AsmToken::EndOfStatement)) {
    // Ensure that operands are separated by commas
    if (First) {
      First = false;
    } else {
      SMLoc Loc = getLexer().getLoc();
      if (Parser.getTok().isNot(AsmToken::Comma)) {
        Parser.eatToEndOfStatement();
        return Error(Loc, "missing comma between operands");
      }
      Parser.Lex();
    }

    // Try to parse one of the assembly operand classes defined in TableGen.
    ParseStatus MatchResult = MatchOperandParserImpl(Operands, Name);
    if (MatchResult.isSuccess()) {
      continue;
    }

    // The TableGen-ed parser couldn't parse anything,
    // operand must be a register.
    MCRegister Reg;
    SMLoc RegStart, RegEnd;
    MatchResult = tryParseRegister(Reg, RegStart, RegEnd);
    if (MatchResult.isSuccess()) {
      Operands.push_back(CDMOperand::createReg(Reg, RegStart, RegEnd));
      continue;
    }

    // Couldn't parse a register, we have an invalid operand
    SMLoc Loc = getLexer().getLoc();
    Parser.eatToEndOfStatement();
    return Error(Loc, "unexpected token parsing operands");
  }

  // Eat EndOfStatement.
  Parser.Lex();
  return false;
}

bool CDMAsmParser::emit(MCInst &Inst, SMLoc const &Loc, MCStreamer &Out) const {
  Inst.setLoc(Loc);
  Out.emitInstruction(Inst, STI);

  return false;
}

bool CDMAsmParser::invalidOperandError(SMLoc Loc, OperandVector &Operands,
                                       uint64_t ErrorInfo) {
  SMLoc ErrorLoc = Loc;
  char const *Diag = 0;

  if (ErrorInfo != ~0U) {
    if (ErrorInfo >= Operands.size()) {
      Diag = "too few operands for instruction.";
    } else {
      ErrorLoc = ((CDMOperand &)*Operands[ErrorInfo]).getStartLoc();
    }
  }
  if (!Diag) {
    Diag = "invalid operand for instruction";
  }
  return Error(ErrorLoc, Diag);
}

bool CDMAsmParser::outOfRangeError(
    OperandVector &Operands, uint64_t ErrorInfo, int64_t Lower, int64_t Upper,
    StringRef Msg = "immediate must be an integer in the range") {
  if (ErrorInfo >= Operands.size()) {
    ErrorInfo = Operands.size() - 1;
  }
  SMLoc Loc = ((CDMOperand &)*Operands[ErrorInfo]).getStartLoc();
  return Error(Loc, Msg + " [" + Twine(Lower) + ", " + Twine(Upper) + "]");
}

bool CDMAsmParser::matchAndEmitInstruction(SMLoc Loc, unsigned &Opcode,
                                           OperandVector &Operands,
                                           MCStreamer &Out, uint64_t &ErrorInfo,
                                           bool MatchingInlineAsm) {
  MCInst Inst;
  unsigned MatchResult =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);

  switch (MatchResult) {
  case Match_Success: {
    return emit(Inst, Loc, Out);
  }
  case Match_MissingFeature: {
    return Error(Loc,
                 "instruction requires a CPU feature not currently enabled");
  }
  case Match_InvalidOperand: {
    return invalidOperandError(Loc, Operands, ErrorInfo);
  }
  case Match_MnemonicFail: {
    return Error(Loc, "invalid instruction mnemonic");
  }
  case Match_InvalidImmSym: {
    return outOfRangeError(
        Operands, ErrorInfo, INT16_MIN, UINT16_MAX,
        "immediate must be a symbol or an integer in the range");
  }
  case Match_InvalidImm9: {
    return outOfRangeError(Operands, ErrorInfo, -512, 511);
  }
  case Match_InvalidImm9Even: {
    return outOfRangeError(
        Operands, ErrorInfo, -1024, 1022,
        "immediate must be a multiple of 2 bytes in the range");
  }
  case Match_InvalidImm6: {
    return outOfRangeError(Operands, ErrorInfo, -64, 63);
  }
  case Match_InvalidImm6Even: {
    return outOfRangeError(
        Operands, ErrorInfo, -128, 126,
        "immediate must be a multiple of 2 bytes in the range");
  }
  case Match_InvalidInterruptVec: {
    return outOfRangeError(Operands, ErrorInfo, 0, 255);
  }
  case Match_InvalidShamt: {
    return outOfRangeError(Operands, ErrorInfo, 1, 8);
  }
  default:
    return true;
  }
}
