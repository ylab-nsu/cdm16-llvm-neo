#ifndef LLVM_LIB_TARGET_CDM_MCTARGETDESC_CDMTARGETSTREAMER_H
#define LLVM_LIB_TARGET_CDM_MCTARGETDESC_CDMTARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/FormattedStream.h"

#include "CDMAsmStreamer.h"

namespace llvm {
class CDMAsmStreamer;

class CDMTargetStreamer : public MCTargetStreamer {
public:
  CDMTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

  virtual void emitRsect(const Twine &Name) {}
  virtual void emitDbgSource(unsigned FileIndex, const Twine &FileName) {}
  virtual void emitDbgLoc(unsigned Index, unsigned Line, unsigned Column) {}
  virtual void emitExtList() {}
  virtual void emitEnd() {}
};

class CDMTargetAsmStreamer : public CDMTargetStreamer {
protected:
  formatted_raw_ostream &OS;

public:
  CDMTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS)
      : CDMTargetStreamer(S), OS(OS) {}

  void emitRsect(const Twine &Name) override {
    getAsmStreamer()->emitRsect(Name);
  }

  void emitDbgSource(unsigned FileIndex, const Twine &FileName) override {
    getAsmStreamer()->emitDbgSource(FileIndex, FileName);
  }

  void emitDbgLoc(unsigned Index, unsigned Line, unsigned Column) override {
    getAsmStreamer()->emitDbgLoc(Index, Line, Column);
  }

  void emitExtList() override { getAsmStreamer()->emitExtList(); }

  void emitEnd() override { getAsmStreamer()->emitEnd(); }

  CDMAsmStreamer *getAsmStreamer() const {
    return static_cast<CDMAsmStreamer *>(&Streamer);
  }
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_CDM_MCTARGETDESC_CDMTARGETSTREAMER_H
