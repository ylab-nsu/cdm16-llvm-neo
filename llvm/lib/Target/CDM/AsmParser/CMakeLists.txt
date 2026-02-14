add_llvm_component_library(LLVMCDMAsmParser
  CDMAsmParser.cpp

  LINK_COMPONENTS
  CDMDesc
  CDMInfo
  CodeGenTypes
  MC
  MCParser
  Support

  ADD_TO_COMPONENT
  CDM
  )
