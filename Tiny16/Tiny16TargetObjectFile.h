#ifndef LLVM_LIB_TARGET_TINY16_TINY16TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_TINY16_TINY16TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

class MCContext;
class TargetMachine;

class Tiny16ELFTargetObjectFile : public TargetLoweringObjectFileELF {
public:
  Tiny16ELFTargetObjectFile() = default;

  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

  const MCExpr *getTTypeGlobalReference(const GlobalValue *GV,
                                        unsigned Encoding,
                                        const TargetMachine &TM,
                                        MachineModuleInfo *MMI,
                                        MCStreamer &Streamer) const override;
};

} // end namespace llvm

#endif
