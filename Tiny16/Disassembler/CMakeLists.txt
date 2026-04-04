add_llvm_component_library(LLVMTiny16Disassembler
  Tiny16Disassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  MC
  Tiny16Info
  Support

  ADD_TO_COMPONENT
  Tiny16
  )
