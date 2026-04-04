#ifndef TINY16MACHINEFUNCTIONINFO_H
#define TINY16MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// Tiny16MachineFunctionInfo - This class is derived from MachineFunction private
/// Tiny16 target-specific information for each MachineFunction.
class Tiny16MachineFunctionInfo : public MachineFunctionInfo {
public:
  Tiny16MachineFunctionInfo() {}
  Tiny16MachineFunctionInfo(const Function &F, const TargetSubtargetInfo *STI) {}

  ~Tiny16MachineFunctionInfo() {}
};
} // End llvm namespace

#endif // TINY16MACHINEFUNCTIONINFO_H
