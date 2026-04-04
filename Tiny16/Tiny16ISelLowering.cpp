#include "Tiny16ISelLowering.h"
#include "Tiny16.h"
#include "Tiny16MachineFunctionInfo.h"
#include "Tiny16Subtarget.h"
#include "Tiny16TargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace llvm;

unsigned Tiny16TargetLowering::getCompOpcode(ISD::CondCode Cond) {
  switch (Cond) {
  default:
    llvm_unreachable("Unsupported condition code");
  case ISD::SETEQ:
    return 0b0000;
  case ISD::SETNE:
    return 0b0001;
  case ISD::SETGT:
    return 0b0010;
  case ISD::SETGE:
    return 0b0011;
  case ISD::SETLT:
    return 0b0100;
  case ISD::SETLE:
    return 0b0101;
  case ISD::SETUGT:
    return 0b0110;
  case ISD::SETUGE:
    return 0b0111;
  case ISD::SETULT:
    return 0b1000;
  case ISD::SETULE:
    return 0b1001;
  }
}

const char *Tiny16TargetLowering::getTargetNodeName(unsigned Opcode) const {
  std::cout << "[TINY16] Tiny16TargetLowering::getTargetNodeName called\n";
  switch (Opcode) {
  default:
    return nullptr;
  case Tiny16ISD::RET_FLAG:
    return "Tiny16ISD::RET_FLAG";
  case Tiny16ISD::LOAD_SYM:
    return "Tiny16ISD::LOAD_SYM";
  case Tiny16ISD::MOVEi16:
    return "Tiny16ISD::MOVEi16";
  case Tiny16ISD::CALL:
    return "Tiny16ISD::CALL";
    // case Tiny16ISD::CMP:
    //   return "Tiny16ISD::CMP";
    // case Tiny16ISD::BR_CC:
    //   return "Tiny16ISD::BR_CC";
  }
}

Tiny16TargetLowering::Tiny16TargetLowering(const Tiny16TargetMachine &Tiny16TM)
    : TargetLowering(Tiny16TM, *Tiny16TM.getSubtargetImpl()),
      Subtarget(*Tiny16TM.getSubtargetImpl()) {
  // Set up the register classes.
  addRegisterClass(MVT::i16, &Tiny16::GPRRegClass);

  // Compute derived properties from the register classes
  computeRegisterProperties(Subtarget.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(Tiny16::FP);

  setSchedulingPreference(Sched::Source);

  // Expand all operations on i32
  for (unsigned Op = 0; Op < ISD::BUILTIN_OP_END; ++Op)
    setOperationAction(Op, MVT::i32, Expand);

  setOperationAction(ISD::UDIV, MVT::i32, Custom);
  setOperationAction(ISD::SDIV, MVT::i32, Custom);

  setOperationAction(ISD::SHL_PARTS, MVT::i16, Expand);
  setOperationAction(ISD::SRA_PARTS, MVT::i16, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i16, Expand);

  // Expand all operations on i64
  for (unsigned Op = 0; Op < ISD::BUILTIN_OP_END; ++Op)
    setOperationAction(Op, MVT::i64, Expand);

  setOperationAction(ISD::BR_CC, MVT::i16, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i16, Custom);
  setOperationAction(ISD::SELECT, MVT::i16, Expand); // No SELECT instruction
  setOperationAction(ISD::SETCC, MVT::i16, Custom);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);

  setOperationAction(ISD::GlobalAddress, MVT::i16, Custom);

  setLoadExtAction(ISD::SEXTLOAD, MVT::i16, MVT::i8, Expand);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i16, MVT::i8, Custom);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i16, MVT::i8, Custom);
  setLoadExtAction(ISD::EXTLOAD, MVT::i16, MVT::i8, Custom);
  setTruncStoreAction(MVT::i16, MVT::i8, Custom);

  // SMUL_LOHI
  setOperationAction(ISD::SMUL_LOHI, MVT::i16, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i16, Expand);

  // ---- DAG COMBINE ---
  // setTargetDAGCombine(ISD::SHL_PARTS);
  // setTargetDAGCombine(ISD::SRA_PARTS);
  // setTargetDAGCombine(ISD::SRL_PARTS);
  setTargetDAGCombine(ISD::SHL);
  setTargetDAGCombine(ISD::SRA);
  setTargetDAGCombine(ISD::SRL);
}

SDValue Tiny16TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerOperation called\n";
  switch (Op.getOpcode()) {
  default:
    llvm_unreachable("Unimplemented operand");
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  case ISD::BR_CC:
    return LowerBR_CC(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSelect_CC(Op, DAG);
  case ISD::SETCC:
    return LowerSetCC(Op, DAG);
  case ISD::LOAD:{
    LoadSDNode *LDNode = cast<LoadSDNode>(Op.getNode());
    ISD::LoadExtType ExtType = LDNode->getExtensionType();
    if (ExtType == ISD::ZEXTLOAD) {
      return LowerLoadZExt(Op, DAG);
    }
    else if (ExtType == ISD::SEXTLOAD) {
      return LowerLoadSExt(Op, DAG);
    }
    else if (ExtType == ISD::EXTLOAD) {
      return LowerLoadZExt(Op, DAG);
    }
    else {
      llvm_unreachable("Unsupported load extension type");
    }
  }
  case ISD::STORE:
    return LowerStoreTrunc(Op, DAG);
  case ISD::UDIV:
    return LowerUDIV(Op, DAG);
  case ISD::SDIV:
    return LowerSDIV(Op, DAG);
  }
}

void Tiny16TargetLowering::ReplaceNodeResults(SDNode* N,
                                SmallVectorImpl<SDValue> &Results,
                                SelectionDAG &DAG) const 
{
  switch (N->getOpcode()) {
    case ISD::UDIV:
      // TODO: Need to make sure that this is only done when both the inputs are i32 types and are expanded from i16. Need to check the ops to make sure it is expanded.
      // Only support i16 division. Check that inputs are i16 or i32 expanded to i16. If so, create new i16 division node, and return result expanded to i32.
      if (N->getValueType(0) == MVT::i32 && N->getOperand(0).getValueType() == MVT::i32) {
        SDValue LHS = N->getOperand(0);
        SDValue RHS = N->getOperand(1);
        SDLoc dl(N);
        // First truncate LHS and RHS to i16 if they are i32
        if (LHS.getValueType() == MVT::i32) {
          LHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, LHS);
        }
        if (RHS.getValueType() == MVT::i32) {
          RHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, RHS);
        }
        SDValue Div = DAG.getNode(ISD::UDIV, dl, MVT::i16, LHS, RHS);
        Results.push_back(DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i32, Div));
      } else {
        llvm_unreachable("Unsupported type for UDIV");
      }
      break;
    case ISD::SDIV:
      // TODO: Need to make sure that this is only done when both the inputs are i32 types and are expanded from i16. Need to check the ops to make sure it is expanded.
      // Only support i16 division. Check that inputs are i16 or i32 expanded to i16. If so, create new i16 division node, and return result expanded to i32.
      if (N->getValueType(0) == MVT::i32 && N->getOperand(0).getValueType() == MVT::i32) {
        SDValue LHS = N->getOperand(0);
        SDValue RHS = N->getOperand(1);
        SDLoc dl(N);
        // First truncate LHS and RHS to i16 if they are i32
        if (LHS.getValueType() == MVT::i32) {
          LHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, LHS);
        }
        if (RHS.getValueType() == MVT::i32) {
          RHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, RHS);
        }
        SDValue Div = DAG.getNode(ISD::SDIV, dl, MVT::i16, LHS, RHS);
        Results.push_back(DAG.getNode(ISD::SIGN_EXTEND, dl, MVT::i32, Div));
      } else {
        llvm_unreachable("Unsupported type for SDIV");
      }
      break;
    default:
      llvm_unreachable("ReplaceNodeResults not implemented for this target!");
  }
}

SDValue Tiny16TargetLowering::LowerGlobalAddress(SDValue Op,
                                                 SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerGlobalAddress called\n";
  EVT VT = Op.getValueType();
  GlobalAddressSDNode *GlobalAddr = cast<GlobalAddressSDNode>(Op.getNode());
  SDValue TargetAddr = DAG.getTargetGlobalAddress(GlobalAddr->getGlobal(), Op,
                                                  MVT::i16);
  return DAG.getNode(Tiny16ISD::LOAD_SYM, Op, VT, TargetAddr);
}

SDValue Tiny16TargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerBR_CC called\n";
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode Cond = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  SDLoc dl(Op);

  // Convert Cond to target specific opcode
  unsigned Tiny16CC = getCompOpcode(Cond);

  // Create a cmp node that produces a glue result
  SDVTList CmpTys = DAG.getVTList(MVT::Glue);
  SmallVector<SDValue, 4> CmpOps;
  CmpOps.push_back(DAG.getConstant(Tiny16CC, dl, MVT::i16));
  CmpOps.push_back(LHS);
  CmpOps.push_back(RHS);
  SDValue Cmp = DAG.getNode(Tiny16ISD::CMP, dl, CmpTys, CmpOps);

  // Make BRC take the chain and the glue produced by CMP.
  SDValue Base = DAG.getRegister(Tiny16::R0, MVT::i16);
  SDVTList BrTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 5> BrOps;
  BrOps.push_back(Chain);              // token chain
  BrOps.push_back(Base);               // base/reg operand
  BrOps.push_back(Dest);               // branch destination
  BrOps.push_back(Cmp.getValue(0));    // glue

  return DAG.getNode(Tiny16ISD::BRC, dl, BrTys, BrOps);
}

SDValue Tiny16TargetLowering::LowerSelect_CC(SDValue Op,
                                             SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerSelect_CC called\n";
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDValue TrueVal = Op.getOperand(2);
  SDValue FalseVal = Op.getOperand(3);
  ISD::CondCode Cond = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDLoc dl(Op);

  // Convert Cond to target specific opcode
  unsigned Tiny16CC = getCompOpcode(Cond);

  // Create a cmp node that produces a glue result
  SDVTList CmpTys = DAG.getVTList(MVT::Glue);
  SmallVector<SDValue, 4> CmpOps;
  CmpOps.push_back(DAG.getConstant(Tiny16CC, dl, MVT::i16));
  CmpOps.push_back(LHS);
  CmpOps.push_back(RHS);
  SDValue Cmp = DAG.getNode(Tiny16ISD::CMP, dl, CmpTys, CmpOps);

  return DAG.getNode(Tiny16ISD::SELC, dl, MVT::i16, TrueVal, FalseVal, Cmp.getValue(0));
}

SDValue Tiny16TargetLowering::LowerSetCC(SDValue Op, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerSetCC called\n";
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  ISD::CondCode Cond = cast<CondCodeSDNode>(Op.getOperand(2))->get();
  SDLoc dl(Op);

  // Convert Cond to target specific opcode
  unsigned Tiny16CC = getCompOpcode(Cond);

  // Create a cmp node that produces a glue result
  SDVTList CmpTys = DAG.getVTList(MVT::Glue);
  SmallVector<SDValue, 4> CmpOps;
  CmpOps.push_back(DAG.getConstant(Tiny16CC, dl, MVT::i16));
  CmpOps.push_back(LHS);
  CmpOps.push_back(RHS);
  SDValue Cmp = DAG.getNode(Tiny16ISD::CMP, dl, CmpTys, CmpOps);

  return DAG.getNode(Tiny16ISD::SETC, dl, MVT::i16, Cmp.getValue(0));
}

SDValue Tiny16TargetLowering::LowerLoadZExt(SDValue Op, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerLoadZExt called\n";
  LoadSDNode *LD = cast<LoadSDNode>(Op.getNode());
  SDValue Chain = LD->getChain();
  SDValue Ptr = LD->getBasePtr();
  SDValue Offset = LD->getOffset();
  SDLoc dl(Op);

  // Currently don't support non undef offsets for loads
  if (!Offset.isUndef()) {
    llvm_unreachable("Non-undef offsets not supported for loads");
  }

  // If the Ptr is a frame index, need to check if it is odd or even.
  if (auto *FIN = dyn_cast<FrameIndexSDNode>(Ptr)) {
    int FrameIndex = FIN->getIndex();
    SDVTList Tys = DAG.getVTList(MVT::i16, MVT::Other);
    if (FrameIndex % 2 == 1) {
      return DAG.getNode(Tiny16ISD::Tiny16LDBZHI, dl, Tys, Chain, Ptr);
    }
    else {
      return DAG.getNode(Tiny16ISD::Tiny16LDBZLO, dl, Tys, Chain, Ptr);
    }
  }
  else if (auto *GA = dyn_cast<GlobalAddressSDNode>(Ptr)) {
    // If the Ptr is a global address, we can directly load from the address. This is used for loading global variables.
    SDVTList Tys = DAG.getVTList(MVT::i16, MVT::Other);
    return DAG.getNode(Tiny16ISD::Tiny16LDBZLO, dl, Tys, Chain, Ptr);
  }
  else {
    llvm_unreachable("Only frame index based loads supported");
  }
}

SDValue Tiny16TargetLowering::LowerLoadSExt(SDValue Op, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerLoadSExt called\n";
  LoadSDNode *LD = cast<LoadSDNode>(Op.getNode());
  SDValue Chain = LD->getChain();
  SDValue Ptr = LD->getBasePtr();
  SDValue Offset = LD->getOffset();
  SDLoc dl(Op);

  // Currently don't support non undef offsets for loads
  if (!Offset.isUndef()) {
    llvm_unreachable("Non-undef offsets not supported for loads");
  }

  // If the Ptr is a frame index, need to check if it is odd or even.
  if (auto *FIN = dyn_cast<FrameIndexSDNode>(Ptr)) {
    int FrameIndex = FIN->getIndex();
    SDVTList Tys = DAG.getVTList(MVT::i16, MVT::Other);
    if (FrameIndex % 2 == 1) {
      return DAG.getNode(Tiny16ISD::Tiny16LDBSHI, dl, Tys, Chain, Ptr);
    }
    else {
      return DAG.getNode(Tiny16ISD::Tiny16LDBSLO, dl, Tys, Chain, Ptr);
    }
  }
  else if (auto *GA = dyn_cast<GlobalAddressSDNode>(Ptr)) {
    // If the Ptr is a global address, we can directly load from the address. This is used for loading global variables.
    SDVTList Tys = DAG.getVTList(MVT::i16, MVT::Other);
    return DAG.getNode(Tiny16ISD::Tiny16LDBSLO, dl, Tys, Chain, Ptr);
  }
  else {
    llvm_unreachable("Only frame index based loads supported");
  }
}

SDValue Tiny16TargetLowering::LowerStoreTrunc(SDValue Op, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerStoreTrunc called\n";
  // Check if it is a truncstore
  StoreSDNode *ST = cast<StoreSDNode>(Op.getNode());
  if (!ST->isTruncatingStore()) {
    llvm_unreachable("Expected a truncating store");
  }
  
  SDValue Chain = ST->getChain();
  SDValue Value = ST->getValue();
  SDValue Ptr = ST->getBasePtr();
  SDValue Offset = ST->getOffset();
  SDLoc dl(Op);

  // Currently don't support non undef offsets for stores
  if (!Offset.isUndef()) {
    llvm_unreachable("Non-undef offsets not supported for stores");
  }

  // If the Ptr is a frame index, need to check if it is odd or even. If odd, change to shift store, otherwise leave the same.
  if (auto *FIN = dyn_cast<FrameIndexSDNode>(Ptr)) {
    int FrameIndex = FIN->getIndex();
    if (FrameIndex % 2 == 1) {
      return DAG.getNode(Tiny16ISD::Tiny16STBHI, dl, MVT::Other, Chain, Value, Ptr);
    }
    else {
      return DAG.getNode(Tiny16ISD::Tiny16STBLO, dl, MVT::Other, Chain, Value, Ptr);
    }
  }
  else if (auto *GA = dyn_cast<GlobalAddressSDNode>(Ptr)) {
    // If the Ptr is a global address, we can directly load from the address. This is used for loading global variables.
    SDVTList Tys = DAG.getVTList(MVT::i16, MVT::Other);
    return DAG.getNode(Tiny16ISD::Tiny16STBLO, dl, Tys, Chain, Ptr);
  }
  else {
    llvm_unreachable("Only frame index based stores supported");
  }
}

SDValue Tiny16TargetLowering::LowerUDIV(SDValue Op, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerUDIV called\n";
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);

  // Check if the operands are i16, if so, insert a 16bit UDIV. If not, check if they are constants and are less than 16 bits. Otherwise, we don't support it.
  if (LHS.getValueType() == MVT::i16 && RHS.getValueType() == MVT::i16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::UDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  // LHS is i16 and rhs is a constant less than 16 bits
  else if (LHS.getValueType() == MVT::i16 && isa<ConstantSDNode>(RHS) && cast<ConstantSDNode>(RHS)->getZExtValue() < 16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::UDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  // RHS is i16 and lhs is a constant less than 16 bits
  else if (RHS.getValueType() == MVT::i16 && isa<ConstantSDNode>(LHS) && cast<ConstantSDNode>(LHS)->getZExtValue() < 16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::UDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  // Both are constants less than 16 bits
  else if (isa<ConstantSDNode>(LHS) && isa<ConstantSDNode>(RHS) && cast<ConstantSDNode>(LHS)->getZExtValue() < 16 && cast<ConstantSDNode>(RHS)->getZExtValue() < 16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::UDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  else {
    llvm_unreachable("Unsupported operands for UDIV");
  }
}

SDValue Tiny16TargetLowering::LowerSDIV(SDValue Op, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerSDIV called\n";
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);

  // Check if the operands are i16, if so, insert a 16bit SDIV. If not, check if they are constants and are less than 16 bits. Otherwise, we don't support it.
  if (LHS.getValueType() == MVT::i16 && RHS.getValueType() == MVT::i16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::SDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  // LHS is i16 and rhs is a constant less than 16 bits
  else if (LHS.getValueType() == MVT::i16 && isa<ConstantSDNode>(RHS) && cast<ConstantSDNode>(RHS)->getSExtValue() < 16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::SDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  // RHS is i16 and lhs is a constant less than 16 bits
  else if (RHS.getValueType() == MVT::i16 && isa<ConstantSDNode>(LHS) && cast<ConstantSDNode>(LHS)->getSExtValue() < 16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::SDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  // Both are constants less than 16 bits
  else if (isa<ConstantSDNode>(LHS) && isa<ConstantSDNode>(RHS) && cast<ConstantSDNode>(LHS)->getSExtValue() < 16 && cast<ConstantSDNode>(RHS)->getSExtValue() < 16) {
    SDVTList Tys = DAG.getVTList(MVT::i16);
    return DAG.getNode(ISD::SDIV, SDLoc(Op), Tys, LHS, RHS);
  }
  else {
    llvm_unreachable("Unsupported operands for SDIV");
  }
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "Tiny16GenCallingConv.inc"

//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// Tiny16 call implementation
SDValue
Tiny16TargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                SmallVectorImpl<SDValue> &InVals) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerCall called\n";
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &dl = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_Tiny16);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getAlignedCallFrameSize();

  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, dl);

  SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
  SmallVector<SDValue, 12> MemOpChains;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue Arg = OutVals[i];

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
    default:
      llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full:
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    }

    // Arguments that can be passed on register must be kept at
    // RegsToPass vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      assert(VA.isMemLoc());
      // Stack arguments not implemented yet
    }
  }

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emitted instructions must be
  // stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add register arguments to inputs
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));
  }

  // Add invalidation of caller-saved registers here
  // R1-R4 are arguments/return but also caller-saved according to md?
  // Let's assume standard mask logic handles it if defined in RegisterInfo.

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  Chain = DAG.getNode(Tiny16ISD::CALL, dl, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(NumBytes, dl, true),
                             DAG.getIntPtrConstant(0, dl, true), InFlag, dl);
  InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, IsVarArg, Ins, dl, DAG,
                         InVals);
}

SDValue Tiny16TargetLowering::LowerCallResult(
    SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const {

  std::cout << "[TINY16] Tiny16TargetLowering::LowerCallResult called\n";

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_Tiny16);

  // Copy all of the result registers out of their specified physreg.
  for (auto &Loc : RVLocs) {
    Chain =
        DAG.getCopyFromReg(Chain, dl, Loc.getLocReg(), Loc.getValVT(), InGlue)
            .getValue(1);
    InGlue = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// Tiny16 formal arguments implementation
SDValue Tiny16TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerFormalArguments called\n";
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Tiny16);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC = &Tiny16::GPRRegClass;
      unsigned Reg = RegInfo.createVirtualRegister(RC);
      RegInfo.addLiveIn(VA.getLocReg(), Reg);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);
      InVals.push_back(ArgValue);
    } else {
      // Stack arguments not implemented
    }
  }
  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool Tiny16TargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool isVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context,
    const Type *RetTy) const {
  std::cout << "[TINY16] Tiny16TargetLowering::CanLowerReturn called\n";
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);

  // // Print the MachineFunction (machine IR)
  // MF.dump();
  // // Print LLVM IR for the underlying Function (if needed)
  // MF.getFunction().print(llvm::errs());

  return CCInfo.CheckReturn(Outs, RetCC_Tiny16);
}

SDValue
Tiny16TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                  bool isVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  const SmallVectorImpl<SDValue> &OutVals,
                                  const SDLoc &dl, SelectionDAG &DAG) const {
  std::cout << "[TINY16] Tiny16TargetLowering::LowerReturn called\n";
  // DAG.dumpDotGraph("C:\\Users\\jazim\\Projects\\opensource\\llvm-project\\DAGS\\lower_return.dot",
  // "Tiny16TargetLowering::LowerReturn entry");
  // DAG.viewGraph("Tiny16TargetLowering::LowerReturn entry");
  if (isVarArg) {
    report_fatal_error("VarArg not supported");
  }

  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_Tiny16);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);

    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode()) {
    RetOps.push_back(Flag);
  }

  return DAG.getNode(Tiny16ISD::RET_FLAG, dl, MVT::Other, RetOps);
}

SDValue Tiny16TargetLowering::PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const {
  std::cout << "[TINY16] Tiny16TargetLowering::PerformDAGCombine called on node: " << N->getOpcode() << "\n";
  switch (N->getOpcode()) {
  default:
    return SDValue();
  // case ISD::SHL_PARTS:
  //   return PerformSHL_PARTSCombine(N, DCI);
  // case ISD::SRA_PARTS:
  //   return PerformSRA_PARTSCombine(N, DCI);
  // case ISD::SRL_PARTS:
  //   return PerformSRL_PARTSCombine(N, DCI);
  case ISD::SHL:
    return PerformSHLCombine(N, DCI);
  case ISD::SRA:
    return PerformSRACombine(N, DCI);
  case ISD::SRL:
    return PerformSRLCombine(N, DCI);
  }
}

// SDValue Tiny16TargetLowering::PerformSHL_PARTSCombine(SDNode *N, DAGCombinerInfo &DCI) const {
//   std::cout << "[TINY16] Tiny16TargetLowering::PerformSHL_PARTSCombine called\n";
//   SelectionDAG &DAG = DCI.DAG;
//   SDLoc dl(N);
  
//   // SHL_PARTS produces two results: [Lo, Hi]
//   // If only the Lo part is used, we can simplify to a regular SHL
  
//   EVT VT = N->getValueType(0);
//   if (VT != MVT::i16) {
//     return SDValue();
//   }
  
//   // Count uses of each result
//   unsigned LoUses = 0, HiUses = 0;
//   for (SDNode::use_iterator UI = N->use_begin(), UE = N->use_end(); UI != UE; ++UI) {
//     SDUse &Use = *UI;
//     unsigned OperandNo = Use.getOperandNo();
//     SDNode *User = Use.getUser();
//     // Get which result of N is being used
//     SDValue OpVal = User->getOperand(OperandNo);
//     if (OpVal.getNode() == N) {
//       unsigned ResNo = OpVal.getResNo();
//       if (ResNo == 0) {
//         ++LoUses;
//       } else {
//         ++HiUses;
//       }
//     }
//   }
  
//   // If only the low part is used, replace with simple SHL
//   if (LoUses > 0 && HiUses == 0) {
//     SDValue Lo = N->getOperand(0);
//     SDValue ShiftAmt = N->getOperand(2);
//     SDValue SimpleSHL = DAG.getNode(ISD::SHL, dl, MVT::i16, Lo, ShiftAmt);
    
//     // Replace all uses of the low result with the simplified SHL
//     DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), SimpleSHL);
    
//     return SDValue(N, 0);
//   }
  
//   return SDValue();
// }

// SDValue Tiny16TargetLowering::PerformSRA_PARTSCombine(SDNode *N, DAGCombinerInfo &DCI) const {
//   std::cout << "[TINY16] Tiny16TargetLowering::PerformSRA_PARTSCombine called\n";
//   SelectionDAG &DAG = DCI.DAG;
//   SDLoc dl(N);
  
//   // SRA_PARTS produces two results: [Lo, Hi]
//   // If only the Lo part is used, we can simplify to a regular SRA
  
//   EVT VT = N->getValueType(0);
//   if (VT != MVT::i16) {
//     return SDValue();
//   }
  
//   // Count uses of each result
//   unsigned LoUses = 0, HiUses = 0;
//   for (SDNode::use_iterator UI = N->use_begin(), UE = N->use_end(); UI != UE; ++UI) {
//     SDUse &Use = *UI;
//     unsigned OperandNo = Use.getOperandNo();
//     SDNode *User = Use.getUser();
//     // Get which result of N is being used
//     SDValue OpVal = User->getOperand(OperandNo);
//     if (OpVal.getNode() == N) {
//       unsigned ResNo = OpVal.getResNo();
//       if (ResNo == 0) {
//         ++LoUses;
//       } else {
//         ++HiUses;
//       }
//     }
//   }
  
//   // If only the low part is used, and the second operand is the sign of the first operand, replace with simple SRA
//   if (LoUses > 0 && HiUses == 0) {
//     SDValue Lo = N->getOperand(0);
//     SDValue Hi = N->getOperand(1);

//     // Check that Hi is the sign extension of Lo (i.e. Hi == (Lo >> 15))
//     if (Hi != DAG.getNode(ISD::SRA, dl, MVT::i16, Lo, DAG.getConstant(15, dl, MVT::i16))) {
//       return SDValue();
//     }
    
//     SDValue ShiftAmt = N->getOperand(2);
//     SDValue SimpleSRA = DAG.getNode(ISD::SRA, dl, MVT::i16, Lo, ShiftAmt);
    
//     // Replace all uses of the low result with the simplified SRA
//     DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), SimpleSRA);
    
//     return SDValue(N, 0);
//   }
  
//   return SDValue();
// }

// SDValue Tiny16TargetLowering::PerformSRL_PARTSCombine(SDNode *N, DAGCombinerInfo &DCI) const {
//   std::cout << "[TINY16] Tiny16TargetLowering::PerformSRL_PARTSCombine called\n";
//   SelectionDAG &DAG = DCI.DAG;
//   SDLoc dl(N);
  
//   // SRL_PARTS produces two results: [Lo, Hi]
//   // If only the Lo part is used, we can simplify to a regular SRL
  
//   EVT VT = N->getValueType(0);
//   if (VT != MVT::i16) {
//     return SDValue();
//   }
  
//   // Count uses of each result
//   unsigned LoUses = 0, HiUses = 0;
//   for (SDNode::use_iterator UI = N->use_begin(), UE = N->use_end(); UI != UE; ++UI) {
//     SDUse &Use = *UI;
//     unsigned OperandNo = Use.getOperandNo();
//     SDNode *User = Use.getUser();
//     // Get which result of N is being used
//     SDValue OpVal = User->getOperand(OperandNo);
//     if (OpVal.getNode() == N) {
//       unsigned ResNo = OpVal.getResNo();
//       if (ResNo == 0) {
//         ++LoUses;
//       } else {
//         ++HiUses;
//       }
//     }
//   }
  
//   // If only the low part is used, and upper is a constant 0, replace with simple SRL
//   if (LoUses > 0 && HiUses == 0) {
//     SDValue Lo = N->getOperand(0);
//     SDValue Hi = N->getOperand(1);

//     // Check that Hi is a constant 0
//     if (Hi.getOpcode() != ISD::Constant || cast<ConstantSDNode>(Hi)->getZExtValue() != 0) {
//       return SDValue();
//     }

//     SDValue ShiftAmt = N->getOperand(2);
//     SDValue SimpleSRL = DAG.getNode(ISD::SRL, dl, MVT::i16, Lo, ShiftAmt);
    
//     // Replace all uses of the low result with the simplified SRL
//     DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), SimpleSRL);
    
//     return SDValue(N, 0);
//   }
  
//   return SDValue();
// }

SDValue Tiny16TargetLowering::PerformSHLCombine(SDNode *N, DAGCombinerInfo &DCI) const {
  std::cout << "[TINY16] Tiny16TargetLowering::PerformSHLCombine called\n";
  SelectionDAG &DAG = DCI.DAG;
  SDLoc dl(N);
  
  EVT VT = N->getValueType(0);
  if (VT == MVT::i16) {
    return SDValue();
  }
  else if (VT != MVT::i32) {
    llvm_unreachable("Unsupported type for SHL");
  }

  // Want to check the pattern to see if the result is always truncated to an i16 or an i8
  for (SDNode::use_iterator UI = N->use_begin(), UE = N->use_end(); UI != UE; ++UI) {
    SDUse &Use = *UI;
    if (Use.getUser()->getOpcode() != ISD::TRUNCATE) {
      return SDValue();
    }
    EVT TruncVT = Use.getUser()->getValueType(0);
    if (TruncVT != MVT::i16 && TruncVT != MVT::i8) {
      return SDValue();
    }
  }
  
  // replace "trunc (shl (i32 X), (i16/i32 Y))" with "zext (shl (trunc X), (trunc Y))"
  SDValue LHS = N->getOperand(0);
  SDValue RHS = N->getOperand(1);
  SDValue TruncLHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, LHS);
  SDValue TruncRHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, RHS);
  SDValue NewSHL = DAG.getNode(ISD::SHL, dl, MVT::i16, TruncLHS, TruncRHS);
  SDValue ZextSHL = DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i32, NewSHL);

  DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), ZextSHL);
  
  return SDValue(N, 0);
}

SDValue Tiny16TargetLowering::PerformSRACombine(SDNode *N, DAGCombinerInfo &DCI) const {
  std::cout << "[TINY16] Tiny16TargetLowering::PerformSRACombine called\n";
  SelectionDAG &DAG = DCI.DAG;
  SDLoc dl(N);
  
  EVT VT = N->getValueType(0);
  if (VT == MVT::i16) {
    return SDValue();
  }
  else if (VT != MVT::i32) {
    llvm_unreachable("Unsupported type for SRA");
  }

  // Want to check the pattern to see if the result is always truncated to an i16 or an i8
  for (SDNode::use_iterator UI = N->use_begin(), UE = N->use_end(); UI != UE; ++UI) {
    SDUse &Use = *UI;
    if (Use.getUser()->getOpcode() != ISD::TRUNCATE) {
      return SDValue();
    }
    EVT TruncVT = Use.getUser()->getValueType(0);
    if (TruncVT != MVT::i16 && TruncVT != MVT::i8) {
      return SDValue();
    }
  }
  
  // replace "trunc (sra sext(i8/i16 X), (i16/i32 Y))" with "anyext (sra (trunc X), (trunc Y))"
  SDValue LHS = N->getOperand(0);
  SDValue RHS = N->getOperand(1);
  // Want to check if the first operand is sign extended from an i8 or i16
  if (LHS.getOpcode() != ISD::SIGN_EXTEND) {
    return SDValue();
  }
  SDValue TruncLHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, LHS);
  SDValue TruncRHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, RHS);
  SDValue NewSRA = DAG.getNode(ISD::SRA, dl, MVT::i16, TruncLHS, TruncRHS);
  SDValue ZextSRA = DAG.getNode(ISD::ANY_EXTEND, dl, MVT::i32, NewSRA);

  DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), ZextSRA);
  
  return SDValue(N, 0);
}

SDValue Tiny16TargetLowering::PerformSRLCombine(SDNode *N, DAGCombinerInfo &DCI) const {
  std::cout << "[TINY16] Tiny16TargetLowering::PerformSRLCombine called\n";
  SelectionDAG &DAG = DCI.DAG;
  SDLoc dl(N);
  
  EVT VT = N->getValueType(0);
  if (VT == MVT::i16) {
    return SDValue();
  }
  else if (VT != MVT::i32) {
    llvm_unreachable("Unsupported type for SRL");
  }

  // Want to check the pattern to see if the result is always truncated to an i16 or an i8
  for (SDNode::use_iterator UI = N->use_begin(), UE = N->use_end(); UI != UE; ++UI) {
    SDUse &Use = *UI;
    if (Use.getUser()->getOpcode() != ISD::TRUNCATE) {
      return SDValue();
    }
    EVT TruncVT = Use.getUser()->getValueType(0);
    if (TruncVT != MVT::i16 && TruncVT != MVT::i8) {
      return SDValue();
    }
  }
  
  // replace "trunc (sra zext(i8/i16 X), (i16/i32 Y))" with "anyext (sra (trunc X), (trunc Y))"
  SDValue LHS = N->getOperand(0);
  SDValue RHS = N->getOperand(1);
  // Want to check if the first operand is zero extended from an i8 or i16
  if (LHS.getOpcode() != ISD::ZERO_EXTEND) {
    return SDValue();
  }
  SDValue TruncLHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, LHS);
  SDValue TruncRHS = DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, RHS);
  SDValue NewSRA = DAG.getNode(ISD::SRA, dl, MVT::i16, TruncLHS, TruncRHS);
  SDValue ZextSRA = DAG.getNode(ISD::ANY_EXTEND, dl, MVT::i32, NewSRA);

  DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), ZextSRA);
  
  return SDValue(N, 0);
}