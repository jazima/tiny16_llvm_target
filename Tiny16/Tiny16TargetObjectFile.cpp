#include "Tiny16TargetObjectFile.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include <iostream>

using namespace llvm;

void Tiny16ELFTargetObjectFile::Initialize(MCContext &Ctx,
                                           const TargetMachine &TM) {
  std::cout << "[TINY16] Tiny16ELFTargetObjectFile::Initialize called\n";
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
}

const MCExpr *Tiny16ELFTargetObjectFile::getTTypeGlobalReference(
    const GlobalValue *GV, unsigned Encoding, const TargetMachine &TM,
    MachineModuleInfo *MMI, MCStreamer &Streamer) const {
  std::cout
      << "[TINY16] Tiny16ELFTargetObjectFile::getTTypeGlobalReference called\n";
  // TODO: Implement properly
  assert(false &&
         "Tiny16ELFTargetObjectFile::getTTypeGlobalReference Unimplemented");
  return TargetLoweringObjectFileELF::getTTypeGlobalReference(GV, Encoding, TM,
                                                              MMI, Streamer);
}
