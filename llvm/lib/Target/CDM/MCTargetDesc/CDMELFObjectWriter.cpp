#include "MCTargetDesc/CDMFixupKinds.h"
#include "MCTargetDesc/CDMMCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

class CDMELFObjectWriter : public MCELFObjectTargetWriter {
public:
  CDMELFObjectWriter(uint8_t OSABI);

  virtual ~CDMELFObjectWriter() = default;

  unsigned getRelocType(const MCFixup &, const MCValue &,
                        bool IsPCRel) const override;
};

CDMELFObjectWriter::CDMELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_CDM, true) {}

unsigned CDMELFObjectWriter::getRelocType(const MCFixup &Fixup,
                                          const MCValue &Target,
                                          bool IsPCRel) const {
  if ((unsigned)Fixup.getKind() != FK_Data_2) {
    llvm_unreachable("Invalid fixup kind");
  }
  return ELF::R_CDM_16;
}

std::unique_ptr<MCObjectTargetWriter> createCDMELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<CDMELFObjectWriter>(OSABI);
}

} // end of namespace llvm
