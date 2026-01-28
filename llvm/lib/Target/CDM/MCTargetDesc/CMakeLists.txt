add_llvm_component_library(LLVMCDMDesc
    CDMMCTargetDesc.cpp
    CDMAsmStreamer.cpp
    CDMMCAsmInfo.cpp
    CDMELFObjectWriter.cpp
    CDMAsmBackend.cpp
    CDMMCCodeEmitter.cpp

    LINK_COMPONENTS
    MC
    CDMInfo
    Support
    CDMInstPrinter
    CDMCodeGen

    ADD_TO_COMPONENT
    CDM
)
