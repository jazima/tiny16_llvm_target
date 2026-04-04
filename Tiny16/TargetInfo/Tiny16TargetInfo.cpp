// Tiny16TargetInfo.cpp
#include "llvm/MC/TargetRegistry.h"
#include "TargetInfo/Tiny16TargetInfo.h"
#include "llvm/Support/Compiler.h"
using namespace llvm;

Target & llvm::getTheTiny16Target() {
  static Target TheTiny16Target;
  return TheTiny16Target;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeTiny16TargetInfo() {
  RegisterTarget<Triple::tiny16, /*HasJIT=*/false> X(
      getTheTiny16Target(), "tiny16", "Tiny 16-bit ISA", "Tiny16");
}
