add_llvm_component_group(CDM)

set(LLVM_TARGET_DEFINITIONS CDM.td)

tablegen(LLVM CDMGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM CDMGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM CDMGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM CDMGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM CDMGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM CDMGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM CDMGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM CDMGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM CDMGenCallingConv.inc -gen-callingconv)
tablegen(LLVM CDMGenInstrInfo.inc -gen-instr-info)

add_public_tablegen_target(CDMCommonTableGen)

add_llvm_target(CDMCodeGen
  CDMTargetMachine.cpp
  CDMAsmPrinter.cpp
  CDMISelDAGToDAG.cpp
  CDMSubtarget.cpp
  CDMTargetObjectFile.cpp
  CDMISelLowering.cpp
  CDMFrameLowering.cpp
  CDMFunctionInfo.cpp
  CDMRegisterInfo.cpp
  CDMInstrInfo.cpp
  CDMMCInstLower.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  CDMDesc
  CDMInfo
  Support
  Target

  ADD_TO_COMPONENT
  CDM
)

#add_subdirectory(Disassembler)
add_subdirectory(AsmParser)
add_subdirectory(TargetInfo)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
