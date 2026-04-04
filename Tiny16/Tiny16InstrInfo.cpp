#include "Tiny16InstrInfo.h"
#include "MCTargetDesc/Tiny16BaseInfo.h"
#include "Tiny16.h"
#include "Tiny16MachineFunctionInfo.h"
#include "Tiny16Subtarget.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include <iostream>

#define GET_INSTRINFO_CTOR_DTOR
#include "Tiny16GenInstrInfo.inc"

using namespace llvm;

// Pin the vtable to this file.
void Tiny16InstrInfo::anchor() {
  std::cout << "[TINY16] Tiny16InstrInfo::anchor called\n";
}

Tiny16InstrInfo::Tiny16InstrInfo(const Tiny16Subtarget &ST)
    : Tiny16GenInstrInfo(ST, RI, Tiny16::ADJCALLSTACKDOWN,
                         Tiny16::ADJCALLSTACKUP),
      RI() {
  std::cout << "[TINY16] Tiny16InstrInfo constructor called\n";
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
Register Tiny16InstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                              int &FrameIndex) const {
  std::cout << "[TINY16] Tiny16InstrInfo::isLoadFromStackSlot called\n";
  assert(0 && "Unimplemented");
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
Register Tiny16InstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                             int &FrameIndex) const {
  std::cout << "[TINY16] Tiny16InstrInfo::isStoreToStackSlot called\n";
  assert(0 && "Unimplemented");
  return 0;
}

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//
//
/// AnalyzeBranch - Analyze the branching code at the end of MBB, returning
/// true if it cannot be understood (e.g. it's a switch dispatch or isn't
/// implemented for a target).  Upon success, this returns false and returns
/// with the following information in various cases:
///
/// 1. If this block ends with no branches (it just falls through to its succ)
///    just return false, leaving TBB/FBB null.
/// 2. If this block ends with only an unconditional branch, it sets TBB to be
///    the destination block.
/// 3. If this block ends with an conditional branch and it falls through to
///    an successor block, it sets TBB to be the branch destination block and a
///    list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
/// 4. If this block ends with an conditional branch and an unconditional
///    block, it returns the 'true' destination in TBB, the 'false' destination
///    in FBB, and a list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
///
/// Note that RemoveBranch and InsertBranch must be implemented to support
/// cases where this method returns success.
///
bool Tiny16InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  std::cout << "[TINY16] Tiny16InstrInfo::analyzeBranch called\n";
  assert(false && "Tiny16InstrInfo::analyzeBranch Unimplemented");
  return false;
}

/// RemoveBranch - Remove the branching code at the end of the specific MBB.
/// This is only invoked in cases where AnalyzeBranch returns success. It
/// returns the number of instructions that were removed.
unsigned Tiny16InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                       int *BytesRemoved) const {
  std::cout << "[TINY16] Tiny16InstrInfo::removeBranch called\n";
  assert(false && "Tiny16InstrInfo::removeBranch Unimplemented");
  return 0;
}

/// InsertBranch - Insert branch code into the end of the specified
/// MachineBasicBlock.  The operands to this method are the same as those
/// returned by AnalyzeBranch.  This is only invoked in cases where
/// AnalyzeBranch returns success. It returns the number of instructions
/// inserted.
///
/// It is also invoked by tail merging to add unconditional branches in
/// cases where AnalyzeBranch doesn't apply because there was no original
/// branch to analyze.  At least this much must be implemented, else tail
/// merging needs to be disabled.
unsigned Tiny16InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  std::cout << "[TINY16] Tiny16InstrInfo::insertBranch called\n";
  assert(false && "Tiny16InstrInfo::insertBranch Unimplemented");
  return 0;
}

void Tiny16InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL, Register DestReg,
                                  Register SrcReg, bool KillSrc,
                                  bool RenamableDest, bool RenamableSrc) const {
  std::cout << "[TINY16] Tiny16InstrInfo::copyPhysReg called\n";
  BuildMI(MBB, I, I->getDebugLoc(), get(Tiny16::ADD), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc))
      .addReg(Tiny16::R0);
}

void Tiny16InstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC, Register VReg,
    MachineInstr::MIFlag Flags) const {
  std::cout << "[TINY16] Tiny16InstrInfo::storeRegToStackSlot called\n";
  BuildMI(MBB, I, I->getDebugLoc(), get(Tiny16::STRI))
    .addReg(SrcReg, getKillRegState(isKill))
    .addFrameIndex(FrameIndex).addImm(0);
}

void Tiny16InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                           MachineBasicBlock::iterator I,
                                           Register DestReg, int FrameIndex,
                                           const TargetRegisterClass *RC,
                                           Register VReg,
                                           MachineInstr::MIFlag Flags) const {
  std::cout << "[TINY16] Tiny16InstrInfo::loadRegFromStackSlot called\n";
  // Make sure MMO 
  BuildMI(MBB, I, I->getDebugLoc(), get(Tiny16::LDRI), DestReg)
      .addFrameIndex(FrameIndex).addImm(0);
}

bool Tiny16InstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  std::cout << "[TINY16] Tiny16InstrInfo::expandPostRAPseudo called\n";

  // Helper lambda to materialize address offset
  auto getAddressOffsetReg = [&](const MachineOperand &BaseMO, const MachineOperand &OffMO,
                           DebugLoc DL, MachineBasicBlock &MBB) -> unsigned {
    // Addresses comnig in are byte aligned. Need to convert to word offset by dividing by 2.
    const unsigned Offset = OffMO.getImm() >> 1;

    if (Offset != 0) {
      const unsigned Lo = Offset & 0xff;
      const unsigned Hi = (Offset >> 8) & 0xff;
      BuildMI(MBB, MI, DL, get(Tiny16::MOVI), Tiny16::RR).addImm(Lo);
      if (Hi != 0) {
        BuildMI(MBB, MI, DL, get(Tiny16::MOVHI), Tiny16::RR).addImm(Hi);
      }
      return Tiny16::RR;
    } else {
      return Tiny16::R0;
    }
  };
  
  // Helper lambda to materialize address: if offset is non-zero, compute base+offset in RR,
  // otherwise return base register directly.
  auto getAddressReg = [&](const MachineOperand &BaseMO, const MachineOperand &OffMO,
                           DebugLoc DL, MachineBasicBlock &MBB) -> unsigned {
    auto offsetReg = getAddressOffsetReg(BaseMO, OffMO, DL, MBB);
    if (offsetReg != Tiny16::R0) {
      BuildMI(MBB, MI, DL, get(Tiny16::ADD), offsetReg)
          .addReg(BaseMO.getReg())
          .addReg(offsetReg);
      return offsetReg;
    }
    return BaseMO.getReg();
  };
  
  switch (MI.getOpcode()) {
    default:
      return false;
    case Tiny16::MOVI16:{
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned DstReg = MI.getOperand(0).getReg();
      const bool DstIsDead = MI.getOperand(0).isDead();

      const MachineOperand &MO = MI.getOperand(1);

      auto LO = BuildMI(MBB, MI, DL, get(Tiny16::MOVI), DstReg);

      if (MO.isImm()) {
        const unsigned Imm = MO.getImm();
        const unsigned Lo = Imm & 0xff;
        const unsigned Hi = (Imm >> 8) & 0xff;
        LO = LO.addImm(Lo);
        // Add a HI if the high bits are not zero, otherwise just emit a LO.
        if (Hi != 0) {
          auto HI = BuildMI(MBB, MI, DL, get(Tiny16::MOVHI))
                      .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
                      .addReg(DstReg);
          HI = HI.addImm(Hi);
        }
      } else if (MO.isGlobal()) {
        auto HI = BuildMI(MBB, MI, DL, get(Tiny16::MOVHI))
                    .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
                    .addReg(DstReg);
        const GlobalValue *GV = MO.getGlobal();
        const unsigned TF = MO.getTargetFlags();
        LO = LO.addGlobalAddress(GV, MO.getOffset(), TF | Tiny16II::MO_LO8);
        HI = HI.addGlobalAddress(GV, MO.getOffset(), TF | Tiny16II::MO_HI8);
      }
      else if (MO.isSymbol()) {
        auto HI = BuildMI(MBB, MI, DL, get(Tiny16::MOVHI))
                    .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
                    .addReg(DstReg);
        const char *Sym = MO.getSymbolName();
        const unsigned TF = MO.getTargetFlags();
        LO = LO.addExternalSymbol(Sym, TF | Tiny16II::MO_LO8);
        HI = HI.addExternalSymbol(Sym, TF | Tiny16II::MO_HI8);
      } else {
        llvm_unreachable("Unsupported operand for MOVI16");
      }

      MBB.erase(MI);
      return true;
    }
    case Tiny16::RET: {
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      // Expand RET pseudo into a branch to RA.
      // First add a cmpeq r0, r0 to set flag
      BuildMI(MBB, MI, DL, get(Tiny16::CMPEQ)).addReg(Tiny16::R0).addReg(Tiny16::R0);
      BuildMI(MBB, MI, DL, get(Tiny16::BRC)).addReg(Tiny16::RA).addReg(Tiny16::R0);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::CALL: {
      // Expand CALL into a software link sequence.
      // Save current RA on the stack, set RA to the post-call address, branch
      // through the callee register, then restore RA.
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();
      const MachineOperand &TargetMO = MI.getOperand(0);

      if (!TargetMO.isReg())
        llvm_unreachable("CALL expects the callee in a register");

      // Push old RA. Keep the frame pointer 4byte aligned.
      BuildMI(MBB, MI, DL, get(Tiny16::ADDI), Tiny16::FP)
        .addReg(Tiny16::FP)
        .addImm(-2);

      BuildMI(MBB, MI, DL, get(Tiny16::STRR))
        .addReg(Tiny16::RA)
        .addReg(Tiny16::FP)
        .addReg(Tiny16::R0);

      // Each Tiny16 instruction is 2 bytes. After CMPEQ and BRC execute,
      // control should resume at the RA restore sequence below.
      BuildMI(MBB, MI, DL, get(Tiny16::ADDI), Tiny16::RA)
          .addReg(Tiny16::PC)
          .addImm(3);

      BuildMI(MBB, MI, DL, get(Tiny16::CMPEQ))
          .addReg(Tiny16::R0)
          .addReg(Tiny16::R0);

      BuildMI(MBB, MI, DL, get(Tiny16::BRC))
          .addReg(TargetMO.getReg(), getKillRegState(TargetMO.isKill()))
          .addReg(Tiny16::R0);

      // Pop old RA.
      BuildMI(MBB, MI, DL, get(Tiny16::LDRR), Tiny16::RA)
        .addReg(Tiny16::FP)
        .addReg(Tiny16::R0);

      BuildMI(MBB, MI, DL, get(Tiny16::ADDI), Tiny16::FP)
        .addReg(Tiny16::FP)
        .addImm(2);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::LDRI: {
      // Replace with LDRR. Store Imm in a reserved register (rRR) and use that as the source for LDRR.
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned DstReg = MI.getOperand(0).getReg();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("LDRI expects (dst, base reg, immediate offset)");

      // Determine which register to use for the offset.
      // If immediate is zero, use R0 (hardwired zero). Otherwise, materialize in RR.
      unsigned OffsetReg = getAddressOffsetReg(BaseMO, OffMO, DL, MBB);

      // Expand pseudo load with base+imm into base+reg addressing.
      BuildMI(MBB, MI, DL, get(Tiny16::LDRR), DstReg)
          .addReg(BaseMO.getReg())
          .addReg(OffsetReg);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::STRI: {
      // Replace with STRR. Store Imm in a reserved register (rRR) and use
      // that as the source offset register for STRR.
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned ValReg = MI.getOperand(0).getReg();
      const bool ValIsKill = MI.getOperand(0).isKill();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("STRI expects (val reg, base reg, immediate offset)");

      // Determine which register to use for the offset.
      // If immediate is zero, use R0 (hardwired zero). Otherwise, materialize in RR.
      unsigned OffsetReg = getAddressOffsetReg(BaseMO, OffMO, DL, MBB);

      // Expand pseudo store with base+imm into base+reg addressing.
      BuildMI(MBB, MI, DL, get(Tiny16::STRR))
          .addReg(ValReg, getKillRegState(ValIsKill))
          .addReg(BaseMO.getReg())
          .addReg(OffsetReg);

      MBB.erase(MI);
      return true;
    }

    // 8-bit Loads and Stores
    case Tiny16::STBLO_PSEUDO:{
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned ValReg = MI.getOperand(0).getReg();
      const bool ValIsKill = MI.getOperand(0).isKill();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("STBLO_PSEUDO expects (val reg, base reg, immediate offset)");

      unsigned AddrReg = getAddressReg(BaseMO, OffMO, DL, MBB);
      BuildMI(MBB, MI, DL, get(Tiny16::STBLO))
          .addReg(ValReg, getKillRegState(ValIsKill))
          .addReg(AddrReg);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::STBHI_PSEUDO:{
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned ValReg = MI.getOperand(0).getReg();
      const bool ValIsKill = MI.getOperand(0).isKill();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("STBHI_PSEUDO expects (val reg, base reg, immediate offset)");

      unsigned AddrReg = getAddressReg(BaseMO, OffMO, DL, MBB);
      BuildMI(MBB, MI, DL, get(Tiny16::STBHI))
          .addReg(ValReg, getKillRegState(ValIsKill))
          .addReg(AddrReg);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::LDBZLO_PSEUDO:{
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned DstReg = MI.getOperand(0).getReg();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("LDBZLO_PSEUDO expects (dst reg, base reg, immediate offset)");

      unsigned AddrReg = getAddressReg(BaseMO, OffMO, DL, MBB);
      BuildMI(MBB, MI, DL, get(Tiny16::LDBZLO), DstReg)
          .addReg(AddrReg);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::LDBZHI_PSEUDO:{
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned DstReg = MI.getOperand(0).getReg();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("LDBZHI_PSEUDO expects (dst reg, base reg, immediate offset)");

      unsigned AddrReg = getAddressReg(BaseMO, OffMO, DL, MBB);
      BuildMI(MBB, MI, DL, get(Tiny16::LDBZHI), DstReg)
          .addReg(AddrReg);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::LDBSLO_PSEUDO:{
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned DstReg = MI.getOperand(0).getReg();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("LDBSLO_PSEUDO expects (dst reg, base reg, immediate offset)");

      unsigned AddrReg = getAddressReg(BaseMO, OffMO, DL, MBB);
      BuildMI(MBB, MI, DL, get(Tiny16::LDBSLO), DstReg)
          .addReg(AddrReg);

      MBB.erase(MI);
      return true;
    }
    case Tiny16::LDBSHI_PSEUDO:{
      DebugLoc DL = MI.getDebugLoc();
      MachineBasicBlock &MBB = *MI.getParent();

      const unsigned DstReg = MI.getOperand(0).getReg();
      const MachineOperand &BaseMO = MI.getOperand(1);
      const MachineOperand &OffMO = MI.getOperand(2);

      if (!BaseMO.isReg() || !OffMO.isImm())
        llvm_unreachable("LDBSHI_PSEUDO expects (dst reg, base reg, immediate offset)");

      unsigned AddrReg = getAddressReg(BaseMO, OffMO, DL, MBB);
      BuildMI(MBB, MI, DL, get(Tiny16::LDBSHI), DstReg)
          .addReg(AddrReg);

      MBB.erase(MI);
      return true;
    }
  }
}
