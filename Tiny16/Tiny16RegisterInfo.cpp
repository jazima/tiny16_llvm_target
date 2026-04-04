#include "Tiny16RegisterInfo.h"
#include "Tiny16.h"
#include "Tiny16Subtarget.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include <iostream>

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "Tiny16GenRegisterInfo.inc"

Tiny16RegisterInfo::Tiny16RegisterInfo() : Tiny16GenRegisterInfo(Tiny16::RA) {}

// Add debug print in constructor
// (Keep trivial constructor body semantics)

const uint16_t *
Tiny16RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  std::cout << "[TINY16] Tiny16RegisterInfo::getCalleeSavedRegs called\n";
  return CSR_SaveList;
}

BitVector Tiny16RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  Reserved.set(Tiny16::R0);
  Reserved.set(Tiny16::RCMP);
  Reserved.set(Tiny16::RR);
  Reserved.set(Tiny16::RA);
  Reserved.set(Tiny16::PC);
  std::cout << "[TINY16] Tiny16RegisterInfo::getReservedRegs called\n";
  return Reserved;
}

const uint32_t *
Tiny16RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                         CallingConv::ID) const {
  std::cout << "[TINY16] Tiny16RegisterInfo::getCallPreservedMask called\n";
  return CSR_RegMask;
}

bool Tiny16RegisterInfo::requiresRegisterScavenging(
    const MachineFunction &MF) const {
  std::cout
      << "[TINY16] Tiny16RegisterInfo::requiresRegisterScavenging called\n";
  return true;
}

bool Tiny16RegisterInfo::trackLivenessAfterRegAlloc(
    const MachineFunction &MF) const {
  std::cout
      << "[TINY16] Tiny16RegisterInfo::trackLivenessAfterRegAlloc called\n";
  return true;
}

bool Tiny16RegisterInfo::useFPForScavengingIndex(
    const MachineFunction &MF) const {
  std::cout << "[TINY16] Tiny16RegisterInfo::useFPForScavengingIndex called\n";
  return false;
}

bool Tiny16RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                             int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  std::cout << "[TINY16] Tiny16RegisterInfo::eliminateFrameIndex called\n";
  MachineInstr &MI = *II;
  const MachineFunction &MF = *MI.getParent()->getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineOperand &FIOp = MI.getOperand(FIOperandNum);
  unsigned FI = FIOp.getIndex();

  // Determine if we can eliminate the index from this kind of instruction.
  unsigned ImmOpIdx = 0;
  switch (MI.getOpcode()) {
  default:
    // Not supported yet.
    break;
  case Tiny16::LDRI:
  case Tiny16::STRI:
  case Tiny16::STBLO_PSEUDO:
  case Tiny16::STBHI_PSEUDO:
  case Tiny16::LDBZLO_PSEUDO:
  case Tiny16::LDBZHI_PSEUDO:
  case Tiny16::LDBSLO_PSEUDO:
  case Tiny16::LDBSHI_PSEUDO:
    ImmOpIdx = FIOperandNum + 1;
    break;
  }

  // FIXME: check the size of offset.
  MachineOperand &ImmOp = MI.getOperand(ImmOpIdx);
  int Offset = MFI.getObjectOffset(FI) + MFI.getStackSize() + ImmOp.getImm();
  FIOp.ChangeToRegister(Tiny16::FP, false);
  ImmOp.setImm(Offset);

  return false;
}

Register Tiny16RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  std::cout << "[TINY16] Tiny16RegisterInfo::getFrameRegister called\n";
  return Tiny16::FP;
}
