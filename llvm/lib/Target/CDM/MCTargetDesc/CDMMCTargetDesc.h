#ifndef LLVM_CDMMCTARGETDESC_H
#define LLVM_CDMMCTARGETDESC_H

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
MCCodeEmitter *createCDMMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx);

std::unique_ptr<MCObjectTargetWriter> createCDMELFObjectWriter(uint8_t OSABI);

MCAsmBackend *createCDMAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const MCTargetOptions &TO);
} // end namespace llvm

#define GET_REGINFO_ENUM
#include "CDMGenRegisterInfo.inc"

// Defines symbolic names for the Cpu0 instructions.
#define GET_INSTRINFO_ENUM
#include "CDMGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "CDMGenSubtargetInfo.inc"

#endif // LLVM_CDMMCTARGETDESC_H
