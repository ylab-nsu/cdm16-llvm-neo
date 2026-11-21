add_llvm_component_library(LLVMCDMDisassembler
  CDMDisassembler.cpp

  LINK_COMPONENTS
  MC
  MCDisassembler
  CDMInfo
  Support

  ADD_TO_COMPONENT
  CDM
)
