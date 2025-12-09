#include "CDMMCTargetDesc.h"
#include "CDMAsmStreamer.h"
#include "CDMTargetStreamer.h"
#include "CDMMCAsmInfo.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
// #include "CDMTarg"

#include "InstPrinter/CDMInstPrinter.h"

#define GET_INSTRINFO_MC_DESC
#include "CDMGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "CDMGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "CDMAsmPrinter.h"
#include "CDMGenRegisterInfo.inc"

namespace llvm {
Target &getTheCDMTarget();
} // namespace llvm
using namespace llvm;

static MCAsmInfo *createCDMMCAsmInfo(const MCRegisterInfo &MRI,
                                     const Triple &TT,
                                     const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new CDMMCAsmInfo(TT);

  // TODO: DWARF support
  //  unsigned SP = MRI.getDwarfRegNum(CDM::SP, true);
  //  MCCFIInstruction Inst = MCCFIInstruction::createDefCfaRegister(nullptr,
  //  SP); MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCInstrInfo *createCDMMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitCDMMCInstrInfo(X); // defined in Cpu0GenInstrInfo.inc
  return X;
}

static MCRegisterInfo *createCDMMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  // TODO: figure out what 'RA' does (it is not used in generated function)
  InitCDMMCRegisterInfo(X, CDM::PSR); // defined in Cpu0GenRegisterInfo.inc
  return X;
}

static MCSubtargetInfo *createCDMMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  // Figure out what FS does
  return createCDMMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
  // createCpu0MCSubtargetInfoImpl defined in CDMGenSubtargetInfo.inc
}

static MCInstPrinter *createCDMMCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  return new CDMInstPrinter(MAI, MII, MRI);
}

static MCTargetStreamer *createCDMTargetStreamer(MCStreamer &S,
                                                    formatted_raw_ostream &OS,
                                                    MCInstPrinter *InstPrint) {
  return new CDMTargetStreamer(S, OS);
}

/*static MCTargetStreamer *createCDMMCTargetAsmStreamer(MCStreamer &S,
                                                      formatted_raw_ostream &OS,
                                                      MCInstPrinter *InstPrint) {
  return new CDMMCTargetStreamer(S, OS);
}*/

static MCStreamer *createCDMAsmStreamer(MCContext &Context,
                                          std::unique_ptr<formatted_raw_ostream> OS,
                                          std::unique_ptr<MCInstPrinter> IP,
                                          std::unique_ptr<MCCodeEmitter> CE,
                                          std::unique_ptr<MCAsmBackend> MAB) {
  return new CDMAsmStreamer(Context, std::move(OS), std::move(IP), std::move(CE), std::move(MAB));
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeCDMTargetMC() {
  RegisterMCAsmInfoFn X(getTheCDMTarget(), createCDMMCAsmInfo);

  TargetRegistry::RegisterMCInstrInfo(getTheCDMTarget(), createCDMMCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(getTheCDMTarget(), createCDMMCRegisterInfo);
  TargetRegistry::RegisterMCSubtargetInfo(getTheCDMTarget(),
                                          createCDMMCSubtargetInfo);
  TargetRegistry::RegisterMCInstPrinter(getTheCDMTarget(),
                                        createCDMMCInstPrinter);
  TargetRegistry::RegisterAsmTargetStreamer(getTheCDMTarget(),
                                            createCDMTargetStreamer);
  TargetRegistry::RegisterAsmStreamer(getTheCDMTarget(),
                                      createCDMAsmStreamer);
}
