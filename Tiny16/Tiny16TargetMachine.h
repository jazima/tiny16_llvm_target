#ifndef LLVM_LIB_TARGET_TINY16_TINY16TARGETMACHINE_H
#define LLVM_LIB_TARGET_TINY16_TINY16TARGETMACHINE_H

#include "Tiny16Subtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include <optional>

namespace llvm {

class Tiny16TargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  Tiny16Subtarget Subtarget;

public:
  Tiny16TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);
                     
  ~Tiny16TargetMachine() override {};

  const Tiny16Subtarget *getSubtargetImpl() const {
    return &Subtarget;
  }

  virtual const TargetSubtargetInfo *
  getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_TINY16_TINY16TARGETMACHINE_H