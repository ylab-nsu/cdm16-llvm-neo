include "llvm/Target/Target.td"
include "CDMRegisterInfo.td"
include "CDMInstrInfo.td"
include "CDMCallingConv.td"

def CDMInstrInfo : InstrInfo;

// TODO: fill this out
def CDMGenericItineraries : ProcessorItineraries<[], [], []>;
def : Processor<"cdm", CDMGenericItineraries, []>;

def CDMAsmParser : AsmParser {
  let ShouldEmitMatchRegisterName = 1;
  let ShouldEmitMatchRegisterAltName = 1;
}

def CDM : Target {
  let InstructionSet = CDMInstrInfo;
  let AssemblyParsers = [CDMAsmParser];
}
