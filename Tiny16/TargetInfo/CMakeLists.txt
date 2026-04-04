add_llvm_component_library(LLVMTiny16Info
    Tiny16TargetInfo.cpp

    LINK_COMPONENTS
    MC
    Support

    ADD_TO_COMPONENT
    Tiny16
)
