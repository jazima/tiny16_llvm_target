#include "Tiny16Subtarget.h"
#include "Tiny16.h"
#include "Tiny16TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include <iostream>

#define DEBUG_TYPE "leg-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Tiny16GenSubtargetInfo.inc"

using namespace llvm;

void Tiny16Subtarget::anchor() {
  std::cout << "[TINY16] Tiny16Subtarget::anchor called\n";
}

Tiny16Subtarget::Tiny16Subtarget(const StringRef &CPU, const StringRef &TuneCPU,
                                 const StringRef &FS,
                                 const Tiny16TargetMachine &TM)
    : Tiny16GenSubtargetInfo(TM.getTargetTriple(), CPU, TuneCPU, FS),
      InstrInfo(*this), TLInfo(TM), TSInfo(), FrameLowering() {
  std::cout << "[TINY16] Tiny16Subtarget constructor called\n";
}