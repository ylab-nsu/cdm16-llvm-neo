#include "MCTargetDesc/CDMAsmBackend.h"
#include "CDMFixupKinds.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"

#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

std::unique_ptr<MCObjectTargetWriter>
CDMAsmBackend::createObjectTargetWriter() const {
  return createCDMELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

void CDMAsmBackend::adjustFixupValue(const MCFixup &Fixup,
                                     const MCValue &Target, uint64_t &Value,
                                     MCContext &Ctx) const {
  int64_t &Offset = reinterpret_cast<int64_t &>(Value);
  switch (Fixup.getKind()) {
  default:
    llvm_unreachable("Invalid fixup!");
  case CDM::fixup_branch_imm9:
    // Verify that the value is a multiple of 2.
    // Range should be handled by relaxation.
    if (Value % 2) {
      Ctx.reportError(Fixup.getLoc(), "branch offset not aligned");
    }
    // Instructions are 2-byte aligned, so divide by 2.
    // Also subtract 1 instruction because CDM increments PC after relative
    // branches.
    Offset = Offset / 2 - 1;
    // Keep 9 bits, set sign bit when offset is nonnegative.
    Offset = (Offset & 0x1ff) | (Offset >= 0 ? 0x2000 : 0);
    break;
  case CDM::fixup_call_imm9:
    // Verify that the value is a multiple of 2.
    // Range should be handled by relaxation.
    if (Value % 2) {
      Ctx.reportError(Fixup.getLoc(), "call offset not aligned");
    }
    // Same as before
    Offset = Offset / 2 - 1;
    // Keep 9 bits, set sign bit when offset is negative.
    Offset = (Offset & 0x1ff) | (Offset < 0 ? 0x200 : 0);
    break;
  case CDM::fixup_load_imm6:
    // Keep 6 bits, set sign bit when offset is negative.
    Offset = ((Offset & 0x3f) << 0) | (Offset < 0 ? 0x40 : 0);
    break;
  case FK_Data_2:
    // No need for adjustment. Verify that the value can fit.
    if (!isInt<16>(Value) && !isUInt<16>(Value)) {
      Ctx.reportError(Fixup.getLoc(),
                      "operand value outside of 16-bit integer range");
    }
    break;
  }
}

void CDMAsmBackend::applyFixup(const MCFragment &Fragment, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved) {
  if (!IsResolved) {
    if (Fixup.getKind() == FK_Data_2) {
      Asm->getWriter().recordRelocation(Fragment, Fixup, Target, Value);
    } else {
      getContext().reportError(
          Fixup.getLoc(),
          "only 2-byte absolute address relocations are supported");
      return;
    }
  }

  if (mc::isRelocation(Fixup.getKind()))
    return;

  adjustFixupValue(Fixup, Target, Value, getContext());
  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  unsigned NumBits = Info.TargetSize + Info.TargetOffset;
  auto NumBytes = (NumBits / 8) + ((NumBits % 8) == 0 ? 0 : 1);

  // Mask the value and shift it into position.
  Value &= ~(-1 << Info.TargetSize);
  Value <<= Info.TargetOffset;

  unsigned Offset = Fixup.getOffset();
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the
  // bits from the fixup value.
  for (unsigned I = 0; I < NumBytes; ++I) {
    uint8_t Mask = (((Value >> (I * 8)) & 0xff));
    Data[Offset + I] |= Mask;
  }
}

std::optional<MCFixupKind> CDMAsmBackend::getFixupKind(StringRef Name) const {
  unsigned Type;
  Type = llvm::StringSwitch<unsigned>(Name)
#define ELF_RELOC(X, Y) .Case(#X, Y)
#include "llvm/BinaryFormat/ELFRelocs/CDM.def"
#undef ELF_RELOC
             .Case("BFD_RELOC_NONE", ELF::R_CDM_NONE)
             .Case("BFD_RELOC_16", ELF::R_CDM_16)
             .Default(-1u);
  if (Type != -1u)
    return static_cast<MCFixupKind>(FirstLiteralRelocationKind + Type);
  return std::nullopt;
}

MCFixupKindInfo CDMAsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[CDM::NumTargetFixupKinds] = {
      // name           offset  bits  flags
      {"fixup_call_imm9", 0, 10, 0},   // bit 9 is needed to fix up op_type
      {"fixup_branch_imm9", 0, 14, 0}, // bit 13 is needed to fix up opcode
      {"fixup_load_imm6", 3, 7, 0},    // bit 9 is needed to fix up opcode
  };

  if (mc::isRelocation(Kind))
    return {};

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < CDM::NumTargetFixupKinds &&
         "Invalid kind!");

  return Infos[Kind - FirstTargetFixupKind];
}

bool CDMAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                 const MCSubtargetInfo *STI) const {
  // NOP is bfalse (can be a sequence of 0xFF)
  for (; Count >= 1; Count--) {
    OS.write(0xFF);
  }
  return true;
}

bool CDMAsmBackend::mayNeedRelaxation(unsigned Opcode,
                                      ArrayRef<MCOperand> Operands,
                                      const MCSubtargetInfo &STI) const {
  switch (Opcode) {
  default:
    return false;
  case CDM::JSRImm9:
  case CDM::BCondImm9:
  case CDM::BRImm9:
  case CDM::LDIImm6:
    return true;
  }
}

bool CDMAsmBackend::fixupNeedsRelaxation(const MCFixup &Fixup,
                                         uint64_t Value) const {
  int64_t Offset = int64_t(Value);
  switch (Fixup.getKind()) {
  default:
    return false;
  case CDM::fixup_call_imm9:
  case CDM::fixup_branch_imm9:
    return Offset <= -1024 || Offset > 1024;
  case CDM::fixup_load_imm6:
    return Offset < -64 || Offset > 63;
  }
}

static unsigned getRelaxedOpcode(unsigned Opcode, ArrayRef<MCOperand> Operands,
                                 const MCSubtargetInfo &STI) {
  switch (Opcode) {
  default:
    return Opcode;
  case CDM::JSRImm9:
    return CDM::JSRImm16;
  case CDM::BRImm9:
    return CDM::BRImm16;
  case CDM::BCondImm9:
    return CDM::BCondImm16;
  case CDM::LDIImm6:
    return CDM::LDIImm16;
  }
}

void CDMAsmBackend::relaxInstruction(MCInst &Inst,
                                     const MCSubtargetInfo &STI) const {
  MCInst Res;
  switch (Inst.getOpcode()) {
  default:
    llvm_unreachable("Opcode not expected!");
  case CDM::JSRImm9:
  case CDM::BRImm9: {
    Res.setOpcode(getRelaxedOpcode(Inst.getOpcode(), Inst.getOperands(), STI));
    Res.addOperand(Inst.getOperand(0));
    break;
  }
  case CDM::BCondImm9:
  case CDM::LDIImm6: {
    Res.setOpcode(getRelaxedOpcode(Inst.getOpcode(), Inst.getOperands(), STI));
    Res.addOperand(Inst.getOperand(0));
    Res.addOperand(Inst.getOperand(1));
    break;
  }
  }
  Inst = std::move(Res);
}

MCAsmBackend *createCDMAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const MCTargetOptions &TO) {
  return new CDMAsmBackend(STI.getTargetTriple().getOS());
}

} // end namespace llvm
