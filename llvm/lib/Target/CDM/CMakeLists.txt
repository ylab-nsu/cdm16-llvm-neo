add_llvm_component_group(CDM)

set(LLVM_TARGET_DEFINITIONS CDM.td)



file(REMOVE ${CMAKE_BINARY_DIR}/cdm_tablegen_compile_commands.yml)
function(cdm_tablegen)
    # Get the current set of include paths for this td file.
    cmake_parse_arguments(ARG "" "" "DEPENDS;EXTRA_INCLUDES" ${ARGN})
    get_directory_property(tblgen_includes INCLUDE_DIRECTORIES)
    list(APPEND tblgen_includes ${ARG_EXTRA_INCLUDES})
    # Filter out any empty include items.
    list(REMOVE_ITEM tblgen_includes "")

    # Build the absolute path for the current input file.
    if (IS_ABSOLUTE ${LLVM_TARGET_DEFINITIONS})
        set(LLVM_TARGET_DEFINITIONS_ABSOLUTE ${LLVM_TARGET_DEFINITIONS})
    else()
        set(LLVM_TARGET_DEFINITIONS_ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/${LLVM_TARGET_DEFINITIONS})
    endif()

    # Append the includes used for this file to the tablegen_compile_commands
    # file.
    file(APPEND ${CMAKE_BINARY_DIR}/cdm_tablegen_compile_commands.yml
            "--- !FileInfo:\n"
            "  filepath: \"${LLVM_TARGET_DEFINITIONS_ABSOLUTE}\"\n"
            "  includes: \"${CMAKE_CURRENT_SOURCE_DIR};${tblgen_includes}\"\n"
    )
endfunction()


#tablegen(LLVM SparcGenAsmMatcher.inc -gen-asm-matcher)
#tablegen(LLVM SparcGenAsmWriter.inc -gen-asm-writer)
#tablegen(LLVM SparcGenCallingConv.inc -gen-callingconv)
#tablegen(LLVM SparcGenDAGISel.inc -gen-dag-isel)
#tablegen(LLVM SparcGenDisassemblerTables.inc -gen-disassembler)
#tablegen(LLVM SparcGenInstrInfo.inc -gen-instr-info)
#tablegen(LLVM SparcGenMCCodeEmitter.inc -gen-emitter)
#tablegen(LLVM SparcGenRegisterInfo.inc -gen-register-info)
#tablegen(LLVM SparcGenSubtargetInfo.inc -gen-subtarget)
cdm_tablegen(LLVM CDMGenRegisterInfo.inc -gen-register-info)

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
        CDMIselDAGToDAG.cpp
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
        Core
        MC
        SelectionDAG
        CDMInfo
        CDMDesc
        CDMInstPrinter
        Support
        Target
        TargetParser

        ADD_TO_COMPONENT
        CDM
)

#add_subdirectory(Disassembler)
add_subdirectory(AsmParser)
add_subdirectory(TargetInfo)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
