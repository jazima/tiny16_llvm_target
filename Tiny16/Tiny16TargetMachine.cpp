// Tiny16TargetMachine.cpp
#include "Tiny16TargetMachine.h"
#include "TargetInfo/Tiny16TargetInfo.h"
#include "Tiny16.h"
#include "Tiny16FrameLowering.h"
#include "Tiny16ISelLowering.h"
#include "Tiny16InstrInfo.h"
#include "Tiny16MachineFunctionInfo.h"
#include "Tiny16RegisterInfo.h"
#include "Tiny16TargetObjectFile.h"

#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

#include <iostream>
#include <optional>

using namespace llvm;

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTiny16Target() {
  // Register the target.
  std::cout << "[TINY16] LLVMInitializeTiny16Target called\n";
  RegisterTargetMachine<Tiny16TargetMachine> X(getTheTiny16Target());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeTiny16DAGToDAGISelLegacyPass(PR);
}

// static std::string computeDataLayout(const Triple &TT, StringRef CPU,
//                                      const TargetOptions &Options) {
//   std::cout << "[TINY16] computeDataLayout called\n";
//   // XXX Build the triple from the arguments.
//   // This is hard-coded for now for this example target.
//   return "e-m:e-p:16:16-i1:16:16-i8:16:16-i16:16:16-i32:16:16-i64:16:16-n16";
// }

Tiny16TargetMachine::Tiny16TargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         std::optional<Reloc::Model> RM,
                                         std::optional<CodeModel::Model> CM,
                                         CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, TT.computeDataLayout(), TT, CPU, FS, Options,
                               RM.value_or(Reloc::Static),
                               CM.value_or(CodeModel::Small), OL),
      TLOF(std::make_unique<Tiny16ELFTargetObjectFile>()),
      Subtarget(CPU, "", FS, *this) {
  initAsmInfo();
  std::cout << "[TINY16] Tiny16TargetMachine::Tiny16TargetMachine constructor "
               "called\n";
}

MachineFunctionInfo *Tiny16TargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  std::cout
      << "[TINY16] Tiny16TargetMachine::createMachineFunctionInfo called\n";
  return Tiny16MachineFunctionInfo::create<Tiny16MachineFunctionInfo>(Allocator,
                                                                      F, STI);
}

namespace {
/// Tiny16 Code Generator Pass Configuration Options.
class Tiny16PassConfig : public TargetPassConfig {
public:
  Tiny16PassConfig(Tiny16TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  Tiny16TargetMachine &getTiny16TargetMachine() const {
    return getTM<Tiny16TargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *Tiny16TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Tiny16PassConfig(*this, PM);
}

void Tiny16PassConfig::addIRPasses() {
  std::cout << "[TINY16] Tiny16PassConfig::addIRPasses called\n";
  addPass(createAtomicExpandLegacyPass());

  TargetPassConfig::addIRPasses();
}

bool Tiny16PassConfig::addInstSelector() {
  std::cout << "[TINY16] Tiny16PassConfig::addInstSelector called\n";
  // TODO: Implement instruction selector pass
  addPass(createTiny16ISelDag(getTiny16TargetMachine()));
  return false;
}

void Tiny16PassConfig::addPreEmitPass() {
  std::cout << "[TINY16] Tiny16PassConfig::addPreEmitPass called\n";
  // TODO: Add any target-specific pre-emit passes here.
}