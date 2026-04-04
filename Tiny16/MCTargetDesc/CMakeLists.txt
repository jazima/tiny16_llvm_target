add_llvm_component_library(LLVMTiny16Desc
    Tiny16MCTargetDesc.cpp
    Tiny16MCAsmInfo.cpp
    Tiny16MCCodeEmitter.cpp
    Tiny16AsmBackend.cpp
    Tiny16ELFObjectWriter.cpp

    LINK_COMPONENTS
    MC
    Support
    TargetParser
    Tiny16Info
    Tiny16AsmPrinter

    ADD_TO_COMPONENT
    Tiny16
)
