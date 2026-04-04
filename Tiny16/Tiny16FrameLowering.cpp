#include "Tiny16FrameLowering.h"
#include "Tiny16.h"
#include "Tiny16InstrInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetOptions.h"
#include <algorithm> // std::sort
#include <iostream>

using namespace llvm;

//===----------------------------------------------------------------------===//
// Tiny16FrameLowering:
//===----------------------------------------------------------------------===//
Tiny16FrameLowering::Tiny16FrameLowering()
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(4), 0,
                          Align(4)) {
  std::cout << "[TINY16] Tiny16FrameLowering constructor called\n";
  // Do nothing
}

bool Tiny16FrameLowering::hasFPImpl(const MachineFunction &MF) const {
  std::cout << "[TINY16] Tiny16FrameLowering::hasFPImpl called\n";
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MF.getFrameInfo().hasVarSizedObjects();
}

uint64_t Tiny16FrameLowering::computeStackSize(MachineFunction &MF) const {
  std::cout << "[TINY16] Tiny16FrameLowering::computeStackSize called\n";
  MachineFrameInfo &MFI = MF.getFrameInfo();
  uint64_t StackSize = MFI.getStackSize();
  unsigned StackAlign = getStackAlignment();
  std::cout << "[TINY16] Initial StackSize: " << StackSize << "\n";
  std::cout << "[TINY16] StackAlign: " << StackAlign << "\n";
  // Align the stack size to the required alignment.
  if (StackAlign && (StackSize % StackAlign != 0)) {
    StackSize = ((StackSize + StackAlign - 1) / StackAlign) * StackAlign;
    std::cout << "[TINY16] Aligned StackSize: " << StackSize << "\n";
  }
  StackSize >>= 1; // Convert from bytes to words (2 bytes per word)
  return StackSize;
}

// Materialize an offset for a ADD/SUB stack operation.
// Return zero if the offset fits into the instruction as an immediate,
// or the number of the register where the offset is materialized.
static unsigned materializeOffset(MachineFunction &MF, MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  unsigned Offset) {
  std::cout << "[TINY16] materializeOffset called\n";
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  const uint64_t MaxSubImm = 0x7f;
  if (Offset <= MaxSubImm) {
    // The stack offset fits in the ADD/SUB instruction.
    return 0;
  } else {
    // The stack offset does not fit in the ADD/SUB instruction.
    // Materialize the offset using MOVLO/MOVHI.
    unsigned OffsetReg = Tiny16::RR;
    unsigned Offset = (unsigned)(Offset & 0xffff);
    BuildMI(MBB, MBBI, dl, TII.get(Tiny16::MOVI16), OffsetReg)
        .addImm(Offset)
        .setMIFlag(MachineInstr::FrameSetup);
    return OffsetReg;
  }

  assert(false && "Tiny16FrameLowering::materializeOffset Unimplemented");
  return 0;
}

void Tiny16FrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  std::cout << "[TINY16] Tiny16FrameLowering::emitPrologue called\n";

  // Compute the stack size, to determine if we need a prologue at all.
  uint64_t StackSize = computeStackSize(MF);
  if (!StackSize) {
    return;
  }

  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Adjust the stack pointer.
  unsigned StackReg = Tiny16::FP;
  unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, StackSize);
  if (OffsetReg) {
    BuildMI(MBB, MBBI, dl, TII.get(Tiny16::SUB), StackReg)
        .addReg(StackReg)
        .addReg(OffsetReg)
        .setMIFlag(MachineInstr::FrameSetup);
  } else {
    BuildMI(MBB, MBBI, dl, TII.get(Tiny16::ADDI), StackReg)
        .addReg(StackReg)
        .addImm(static_cast<int64_t>(-StackSize))
        .setMIFlag(MachineInstr::FrameSetup);
  }
}

void Tiny16FrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  std::cout << "[TINY16] Tiny16FrameLowering::emitEpilogue called\n";
  // Compute the stack size, to determine if we need an epilogue at all.
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  DebugLoc dl = MBBI->getDebugLoc();
  uint64_t StackSize = computeStackSize(MF);
  if (!StackSize) {
    return;
  }

  // Restore the stack pointer to what it was at the beginning of the function.
  unsigned StackReg = Tiny16::FP;
  unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, StackSize);
  if (OffsetReg) {
    BuildMI(MBB, MBBI, dl, TII.get(Tiny16::ADD), StackReg)
        .addReg(StackReg)
        .addReg(OffsetReg)
        .setMIFlag(MachineInstr::FrameSetup);
  } else {
    BuildMI(MBB, MBBI, dl, TII.get(Tiny16::ADDI), StackReg)
        .addReg(StackReg)
        .addImm(StackSize)
        .setMIFlag(MachineInstr::FrameSetup);
  }
}

MachineBasicBlock::iterator Tiny16FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
  std::cout << "[TINY16] Tiny16FrameLowering::eliminateCallFramePseudoInstr "
               "called\n";
  if (I->getOpcode() == Tiny16::ADJCALLSTACKUP ||
      I->getOpcode() == Tiny16::ADJCALLSTACKDOWN) {
    return MBB.erase(I);
  }
  assert(false &&
         "Tiny16FrameLowering::eliminateCallFramePseudoInstr Unimplemented");
  return I;
}