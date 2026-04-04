#ifndef TINY16ISELLOWERING_H
#define TINY16ISELLOWERING_H

#include "Tiny16.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

// Forward delcarations
class Tiny16Subtarget;
class Tiny16TargetMachine;

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//
class Tiny16TargetLowering : public TargetLowering {
public:
  explicit Tiny16TargetLowering(const Tiny16TargetMachine &TM);

  /// LowerOperation - Provide custom lowering hooks for some operations.
  virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  // ReplaceNodeResults - Lower the nodes results
  virtual void ReplaceNodeResults(SDNode * /*N*/,
                                  SmallVectorImpl<SDValue> &/*Results*/,
                                  SelectionDAG &/*DAG*/) const override;

  /// getTargetNodeName - This method returns the name of a target specific
  //  DAG node.
  virtual const char *getTargetNodeName(unsigned Opcode) const override;

private:
  const Tiny16Subtarget &Subtarget;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &dl, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &dl,
                      SelectionDAG &DAG) const override;

  SDValue LowerCallResult(SDValue Chain, SDValue InGlue,
                          CallingConv::ID CallConv, bool isVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl,
                          SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      LLVMContext &Context, const Type *RetTy) const override;

  // LowerGlobalAddress - Emit a constant load to the global address.
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerSelect_CC(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerSetCC(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerLoadZExt(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerLoadSExt(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerStoreTrunc(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerUDIV(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerSDIV(SDValue Op, SelectionDAG &DAG) const;

  static unsigned getCompOpcode(ISD::CondCode Cond);

  // --- DAG COMBINE ---
  virtual SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

  // SDValue PerformSHL_PARTSCombine(SDNode *N, DAGCombinerInfo &DCI) const;
  // SDValue PerformSRA_PARTSCombine(SDNode *N, DAGCombinerInfo &DCI) const;
  // SDValue PerformSRL_PARTSCombine(SDNode *N, DAGCombinerInfo &DCI) const;
  SDValue PerformSHLCombine(SDNode *N, DAGCombinerInfo &DCI) const;
  SDValue PerformSRACombine(SDNode *N, DAGCombinerInfo &DCI) const;
  SDValue PerformSRLCombine(SDNode *N, DAGCombinerInfo &DCI) const;
};
} // namespace llvm

#endif // TINY16ISELLOWERING_H
