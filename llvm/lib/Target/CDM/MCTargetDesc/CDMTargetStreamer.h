#ifndef LLVM_LIB_TARGET_CDM_MCTARGETDESC_CDMTARGETSTREAMER_H
#define LLVM_LIB_TARGET_CDM_MCTARGETDESC_CDMTARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {
class CDMAsmStreamer;

class CDMTargetStreamer : public MCTargetStreamer {
protected:
    formatted_raw_ostream &OS;

public:
    CDMTargetStreamer(MCStreamer &S, formatted_raw_ostream &os)
        : MCTargetStreamer(S), OS(os) {}

    void emitRsect(StringRef Name);
    void emitDdgSource();
    void emitDbgLoc(unsigned Index, unsigned Line, unsigned Column);
    void emitExtTable();
    void emitEnd();
    
    void changeSection(const MCSection *CurSection, MCSection *Section,
                     uint32_t SubSection, raw_ostream &OS) override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_CDM_MCTARGETDESC_CDMTARGETSTREAMER_H