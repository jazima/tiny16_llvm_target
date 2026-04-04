#ifndef TARGET_TINY16_H
#define TARGET_TINY16_H

#include "MCTargetDesc/Tiny16MCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class FunctionPass;
class Tiny16TargetMachine;
class PassRegistry;

FunctionPass *createTiny16ISelDag(Tiny16TargetMachine &TM);

void initializeTiny16DAGToDAGISelLegacyPass(PassRegistry &);

} // end namespace llvm;

#endif
