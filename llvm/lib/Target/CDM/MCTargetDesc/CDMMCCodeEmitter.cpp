#include "MCTargetDesc/CDMFixupKinds.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/EndianStream.h"

#include "llvm/Support/ErrorHandling.h"
using namespace llvm;

namespace {
class CDMMCCodeEmitter : public MCCodeEmitter {
  CDMMCCodeEmitter(const CDMMCCodeEmitter &) = delete;
  void operator=(const CDMMCCodeEmitter &) = delete;
  MCContext &Ctx;
  MCInstrInfo const &MCII;

public:
  CDMMCCodeEmitter(MCInstrInfo const &MCII, MCContext &Ctx)
      : Ctx(Ctx), MCII(MCII) {}

  ~CDMMCCodeEmitter() override = default;

  uint64_t getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  /// TableGen'erated function for getting the binary encoding for an
  /// instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

private:
  /// Encodes an immediate operand (symbol only).
  template <unsigned FixupKind, unsigned Offset>
  unsigned encodeBranchTarget(const MCInst &MI, unsigned OpNo,
                              SmallVectorImpl<MCFixup> &Fixups,
                              const MCSubtargetInfo &STI) const;

  /// Encodes an immediate operand.
  template <unsigned FixupKind, unsigned Offset>
  unsigned encodeImm(const MCInst &MI, unsigned OpNo,
                     SmallVectorImpl<MCFixup> &Fixups,
                     const MCSubtargetInfo &STI) const;

  /// Encodes an immediate operand for bit shift instructions.
  unsigned encodeShamt(const MCInst &MI, unsigned OpNo,
                       SmallVectorImpl<MCFixup> &Fixups,
                       const MCSubtargetInfo &STI) const;

  /// Encodes an offset for addsp and frame-relative loads/stores.
  template <unsigned Align>
  signed encodeOffset(const MCInst &MI, unsigned OpNo,
                      SmallVectorImpl<MCFixup> &Fixups,
                      const MCSubtargetInfo &STI) const;

  /// Adjusts the opcode of imm6 and imm9 instrucions
  /// based on the sign of the immediate.
  template <unsigned OpNo, unsigned BitOffset>
  unsigned adjustImmOpCode(const MCInst &MI, unsigned EncodedValue,
                           const MCSubtargetInfo &STI) const;
};

} // end anonymous namespace

static void addFixup(SmallVectorImpl<MCFixup> &Fixups, uint32_t Offset,
                     const MCExpr *Value, uint16_t Kind) {
  bool PCRel;
  switch (Kind) {
  default:
    PCRel = false;
    break;
  case FK_Data_2:
    PCRel = false;
    break;
  case CDM::fixup_call_imm9:
  case CDM::fixup_branch_imm9:
    PCRel = true;
    break;
  }
  Fixups.push_back(MCFixup::create(Offset, Value, Kind, PCRel));
}

uint64_t CDMMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                             const MCOperand &MO,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {

  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return MO.getImm();

  llvm_unreachable("Unhandled expression!");
  return 0;
}

template <unsigned FixupKind, unsigned Offset>
unsigned
CDMMCCodeEmitter::encodeBranchTarget(const MCInst &MI, unsigned OpNo,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  assert(MO.isExpr() && "encodeBranchTarget expects only expressions");
  const MCExpr *Expr = MO.getExpr();

  addFixup(Fixups, Offset, Expr, FixupKind);
  return 0;
}

template <unsigned FixupKind, unsigned Offset>
unsigned CDMMCCodeEmitter::encodeImm(const MCInst &MI, unsigned OpNo,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  // If the destination is an immediate, there is nothing to do.
  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr() && "encodeImm expects only expressions or immediates");
  const MCExpr *Expr = MO.getExpr();

  addFixup(Fixups, Offset, Expr, FixupKind);
  return 0;
}

unsigned CDMMCCodeEmitter::encodeShamt(const MCInst &MI, unsigned OpNo,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  int64_t Value;
  if (!MO.evaluateAsConstantImm(Value)) {
    llvm_unreachable("Unsupported shift amount operand!");
  }
  if (Value == 0) {
    llvm_unreachable("Invalid shift amount!");
  }
  return Value - 1;
}

template <unsigned Align>
signed CDMMCCodeEmitter::encodeOffset(const MCInst &MI, unsigned OpNo,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  int64_t Value;
  if (!MO.evaluateAsConstantImm(Value)) {
    llvm_unreachable("Unsupported offset operand!");
  }
  assert(Value % Align == 0 && "Unaligned offset.");
  return Value / Align;
}

template <unsigned OpNo, unsigned BitOffset>
unsigned CDMMCCodeEmitter::adjustImmOpCode(const MCInst &MI,
                                           unsigned EncodedValue,
                                           const MCSubtargetInfo &STI) const {
  int64_t Value;
  const MCOperand &MO = MI.getOperand(OpNo);
  if (!MO.evaluateAsConstantImm(Value)) {
    llvm_unreachable("Unsupported imm6/imm9 operand!");
  }

  if (Value < 0) {
    EncodedValue += 1 << BitOffset;
  }
  return EncodedValue;
}

void CDMMCCodeEmitter::encodeInstruction(const MCInst &MI,
                                         SmallVectorImpl<char> &CB,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  unsigned Size = Desc.getSize();

  switch (Size) {
  default:
    llvm_unreachable("Unhandled encodeInstruction length!");
  case 2: {
    uint16_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
    support::endian::write<uint16_t>(CB, Bits, llvm::endianness::little);
    break;
  }
  case 4: {
    uint32_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
    support::endian::write<uint32_t>(CB, Bits, llvm::endianness::little);
    break;
  }
  }
}

MCCodeEmitter *llvm::createCDMMCCodeEmitter(const MCInstrInfo &MCII,
                                            MCContext &Ctx) {
  return new CDMMCCodeEmitter(MCII, Ctx);
}

#include "CDMGenMCCodeEmitter.inc"
