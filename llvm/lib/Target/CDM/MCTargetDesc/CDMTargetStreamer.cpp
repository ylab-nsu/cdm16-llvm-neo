#include "CDMTargetStreamer.h"
#include "CDMAsmStreamer.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/FormatVariadic.h"

using namespace llvm;

void CDMTargetStreamer::emitRsect(StringRef Name) {
    OS << "rsect " << Name << "\n\n";
}

void CDMTargetStreamer::emitDdgSource() {
    // emit dbg table
}
    
void CDMTargetStreamer::emitDbgLoc(unsigned Index, unsigned Line, unsigned Column) {
    OS << "; dbg_loc " << Index << ", " << Line << ", " << Column << "\n";
}

void CDMTargetStreamer::emitExtTable() {
    if (auto *AsmStreamer = dyn_cast<CDMAsmStreamer>(&Streamer)) {
        const auto &UsedSymbols = AsmStreamer->getUsedSymbols();
        for (const MCSymbol *Sym : UsedSymbols) {
            if (Sym && !Sym->isDefined()) {
                OS << Sym->getName() << ": ext\n";
            }
        }
    }
}

void CDMTargetStreamer::emitEnd() {
    OS << "end.";
}

void CDMTargetStreamer::changeSection(const MCSection *CurSection,
                                         MCSection *Section,
                                         uint32_t SubSection,
                                         raw_ostream &OS) {
    OS << llvm::formatv("### SECTION: {0}\n", Section->getName());
}