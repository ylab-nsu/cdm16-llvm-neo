#ifndef LLVM_CDMFIXUPKINDS_H
#define LLVM_CDMFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace CDM {

enum Fixups {
  fixup_cdm_imm9_pcrel = FirstTargetFixupKind,
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};

} // namespace CDM
} // namespace llvm

#endif // LLVM_CDMFIXUPKINDS_H
