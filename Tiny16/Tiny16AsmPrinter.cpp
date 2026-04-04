#define DEBUG_TYPE "asm-printer"
#include "InstPrinter/Tiny16InstPrinter.h"
#include "TargetInfo/Tiny16TargetInfo.h"
#include "Tiny16.h"
#include "Tiny16InstrInfo.h"
#include "Tiny16MCInstLower.h"
#include "Tiny16Subtarget.h"
#include "Tiny16TargetMachine.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include <algorithm>
#include <cctype>
#include <iostream>
using namespace llvm;

namespace {
class Tiny16AsmPrinter : public AsmPrinter {
  Tiny16MCInstLower MCInstLowering;

public:
  explicit Tiny16AsmPrinter(TargetMachine &TM,
                            std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MCInstLowering(*this) {}

  StringRef getPassName() const override { return "Tiny16 Assembly Printer"; }

  void EmitFunctionEntryLabel();
  void emitInstruction(const MachineInstr *MI) override;
  void emitFunctionBodyStart() override;
};
} // end of anonymous namespace

void Tiny16AsmPrinter::emitFunctionBodyStart() {
  std::cout << "[TINY16] Tiny16AsmPrinter::emitFunctionBodyStart called\n";
  MCInstLowering.Initialize(&MF->getContext());
}

void Tiny16AsmPrinter::EmitFunctionEntryLabel() {
  std::cout << "[TINY16] Tiny16AsmPrinter::EmitFunctionEntryLabel called\n";
  OutStreamer->emitLabel(CurrentFnSym);
}

void Tiny16AsmPrinter::emitInstruction(const MachineInstr *MI) {
  std::cout << "[TINY16] Tiny16AsmPrinter::emitInstruction called\n";
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);

  EmitToStreamer(*OutStreamer, TmpInst);
}

// Force static initialization.
extern "C" void LLVMInitializeTiny16AsmPrinter() {
  std::cout << "[TINY16] LLVMInitializeTiny16AsmPrinter called\n";
  RegisterAsmPrinter<Tiny16AsmPrinter> X(getTheTiny16Target());
}
