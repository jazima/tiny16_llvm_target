add_llvm_component_group(Tiny16)

set(LLVM_TARGET_DEFINITIONS Tiny16.td)

tablegen(LLVM Tiny16GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Tiny16GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Tiny16GenCallingConv.inc -gen-callingconv)
tablegen(LLVM Tiny16GenSDNodeInfo.inc -gen-sd-node-info -sdnode-namespace=Tiny16ISD)
tablegen(LLVM Tiny16GenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM Tiny16GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM Tiny16GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM Tiny16GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM Tiny16GenDAGISel.inc -gen-dag-isel)

add_public_tablegen_target(Tiny16CommonTableGen)

add_llvm_target(Tiny16CodeGen
    Tiny16Subtarget.cpp
    Tiny16TargetMachine.cpp
    Tiny16RegisterInfo.cpp
    Tiny16InstrInfo.cpp
    Tiny16FrameLowering.cpp
    Tiny16ISelLowering.cpp
    Tiny16SelectionDAGInfo.cpp
    Tiny16ISelDAGToDAG.cpp
    Tiny16AsmPrinter.cpp
    Tiny16MCInstLower.cpp
    Tiny16TargetObjectFile.cpp

    LINK_COMPONENTS
    AsmPrinter
    CodeGen
    CodeGenTypes
    Core
    MC
    SelectionDAG
    Tiny16Desc
    Tiny16Info
    Tiny16AsmPrinter
    Support
    Target
    TargetParser

    ADD_TO_COMPONENT
    Sparc
)

add_subdirectory(TargetInfo)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(Disassembler)


# LEG CMakeLists.txt (example)

# set(LLVM_TARGET_DEFINITIONS LEG.td)

# tablegen(LLVM LEGGenRegisterInfo.inc -gen-register-info)
# tablegen(LLVM LEGGenInstrInfo.inc -gen-instr-info)
# tablegen(LLVM LEGGenAsmWriter.inc -gen-asm-writer)
# tablegen(LLVM LEGGenDAGISel.inc -gen-dag-isel)
# tablegen(LLVM LEGGenCallingConv.inc -gen-callingconv)
# tablegen(LLVM LEGGenSubtargetInfo.inc -gen-subtarget)
# tablegen(LLVM LEGGenMCCodeEmitter.inc -gen-emitter)
# add_public_tablegen_target(LEGCommonTableGen)

# add_llvm_target(LEGCodeGen
#     LEGSubtarget.cpp
#     LEGTargetMachine.cpp
#     LEGRegisterInfo.cpp
#     LEGInstrInfo.cpp
#     LEGFrameLowering.cpp
#     LEGISelLowering.cpp
#     LEGSelectionDAGInfo.cpp
#     LEGISelDAGToDAG.cpp
#     LEGAsmPrinter.cpp
#     LEGMCInstLower.cpp
# )

# add_subdirectory(InstPrinter)
# add_subdirectory(TargetInfo)
# add_subdirectory(MCTargetDesc)