#include "MCTargetDesc/CDMAsmBackend.h"
#include "CDMFixupKinds.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"

#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"

namespace llvm {

std::unique_ptr<MCObjectTargetWriter>
CDMAsmBackend::createObjectTargetWriter() const {
  return createCDMELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

void CDMAsmBackend::adjustFixupValue(const MCFixup &Fixup,
                                     const MCValue &Target, uint64_t &Value,
                                     MCContext *Ctx) const {
  int64_t &Offset = reinterpret_cast<int64_t&>(Value);
  switch (Fixup.getKind()) {
  default:
    llvm_unreachable("invalid fixup");
  case CDM::fixup_branch_imm9:
    // Instructions are 2-byte aligned, so divide by 2.
    // Also subtract 1 instruction because CDM increments PC after relative
    // branches. Keep 9 bits, set sign bit when offset is nonnegative.
    Offset = ((Offset / 2 - 1) & 0x1ff) | (Offset >= 0 ? 0x2000 : 0);
    break;
  case CDM::fixup_call_imm9:
    // Same as previous.
    // Instructions are 2-byte aligned, so divide by 2.
    // Also subtract 1 instruction because CDM increments PC after relative
    // branches. Keep 9 bits, set sign bit when offset is negative.
    Offset = ((Offset / 2 - 1) & 0x1ff) | (Offset < 0 ? 0x200 : 0);
    break;
  case FK_Data_2:
    // No need for adjustment.
    break;
  }
}

void CDMAsmBackend::applyFixup(const MCFragment &Fragment, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved) {
  if (!IsResolved)
    Asm->getWriter().recordRelocation(Fragment, Fixup, Target, Value);

  adjustFixupValue(Fixup, Target, Value, &getContext());
  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  unsigned NumBits = Info.TargetSize + Info.TargetOffset;
  auto NumBytes = (NumBits / 8) + ((NumBits % 8) == 0 ? 0 : 1);

  // Shift the value into position.
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
      // name                    offset  bits  flags
      {"fixup_call_imm9", 0, 10, 0},   // 10th bit is needed to fix up op_type
      {"fixup_branch_imm9", 0, 14, 0}, // 14th bit is needed to fix up opcode
  };

  // Fixup kinds from .reloc directive are like R_AVR_NONE. They do not require
  // any extra processing.
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
  // Instructions always are at even addresses.  We must be in a data area or
  // be unaligned due to some other reason.
  if (Count % 2) {
    OS.write("\x00", 1);
    Count -= 1;
  }
  // Canonical NOP is bfalse -FF
  for (; Count >= 2; Count -= 2) {
    OS.write("\xff\xdf", 2);
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
  }
}

void CDMAsmBackend::relaxInstruction(MCInst &Inst,
                                     const MCSubtargetInfo &STI) const {
  MCInst Res;
  switch (Inst.getOpcode()) {
  default:
    llvm_unreachable("Opcode not expected!");
  case CDM::BRImm9:
  case CDM::JSRImm9: {
    Res.setOpcode(getRelaxedOpcode(Inst.getOpcode(), Inst.getOperands(), STI));
    Res.addOperand(Inst.getOperand(0));
    break;
  }
  case CDM::BCondImm9: {
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
