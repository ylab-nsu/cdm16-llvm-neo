add_llvm_component_library(LLVMCDMDesc
    CDMMCTargetDesc.cpp
    CDMAsmStreamer.cpp
    CDMTargetStreamer.cpp
        CDMMCAsmInfo.cpp

        LINK_COMPONENTS
        MC
        CDMInfo
        Support
        CDMInstPrinter
        CDMCodeGen

        ADD_TO_COMPONENT
        CDM
)