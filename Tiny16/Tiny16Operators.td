def MoveImm16Ty : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisInt<0>]>;

def SDT_Tiny16CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;

def SDT_Tiny16CallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;

def Tiny16ret : SDNode<"Tiny16ISD::RET_FLAG",
                       SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START",
                           SDT_Tiny16CallSeqStart, [SDNPHasChain, SDNPOutGlue]>;

def callseq_end
    : SDNode<"ISD::CALLSEQ_END",
             SDT_Tiny16CallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def SDT_Tiny16Call : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;

//===----------------------------------------------------------------------===//
// Custom SDNodes.
//===----------------------------------------------------------------------===//

def load_sym : SDNode<"Tiny16ISD::LOAD_SYM", SDTIntUnaryOp>;

def movei16 : SDNode<"Tiny16ISD::MOVEi16", MoveImm16Ty>;

def Tiny16call
    : SDNode<"Tiny16ISD::CALL", SDT_Tiny16Call,
             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;

// Takes as input two i16 values and a cmp_op and outputs a boolean value
def Tiny16cmp
    : SDNode<"Tiny16ISD::CMP",
             SDTypeProfile<
                 0, 3, [SDTCisVT<0, i16>, SDTCisVT<1, i16>, SDTCisVT<2, i16>]>,
             [SDNPOutGlue]>;

// Takes as input two i16 values. Is expected to be used after a cmp node
def Tiny16brc
    : SDNode<"Tiny16ISD::BRC",
             SDTypeProfile<
                 0, 2, [SDTCisVT<0, i16>, SDTCisVT<1, i16>]>, // Base, Target
             [SDNPHasChain, SDNPOptInGlue]>;

// Outputs a i1 value. Is expected to be used after a cmp node
def Tiny16setc
    : SDNode<"Tiny16ISD::SETC",
             SDTypeProfile<1, 0, [SDTCisVT<0, i16>]>, [SDNPOptInGlue]>;

// Takes as input two i16 values, and outputs an i16 value. Is expected to be
// used after a cmp node
def Tiny16selc : SDNode<"Tiny16ISD::SELC",
                        SDTypeProfile<1, 2,
                                      [SDTCisVT<0, i16>, SDTCisVT<1, i16>,
                                       SDTCisVT<2, i16>]>, // TrueVal, FalseVal
                        [SDNPOptInGlue]>;

// Byte aligned laods and stores
// Load lower 8 bits of address, and store in a register

def Tiny16STBLO : SDNode<"Tiny16ISD::Tiny16STBLO", SDTypeProfile<0, 2,
                                      [SDTCisVT<0, i16>, SDTCisVT<1, i16>]>,
                        [SDNPHasChain]>;

def Tiny16STBHI : SDNode<"Tiny16ISD::Tiny16STBHI", SDTypeProfile<0, 2,
                                      [SDTCisVT<0, i16>, SDTCisVT<1, i16>]>,
                        [SDNPHasChain]>;

// Load sign extended
def Tiny16LDBZLO : SDNode<"Tiny16ISD::Tiny16LDBZLO", SDTypeProfile<1, 1,
                                      [SDTCisVT<0, i16>, SDTCisVT<1, i16>]>,
                        [SDNPHasChain]>;

def Tiny16LDBZHI : SDNode<"Tiny16ISD::Tiny16LDBZHI", SDTypeProfile<1, 1,
                                      [SDTCisVT<0, i16>, SDTCisVT<1, i16>]>,
                        [SDNPHasChain]>;

// Load zero extended
def Tiny16LDBSLO : SDNode<"Tiny16ISD::Tiny16LDBSLO", SDTypeProfile<1, 1,
                                      [SDTCisVT<0, i16>, SDTCisVT<1, i16>]>,
                        [SDNPHasChain]>;

def Tiny16LDBSHI : SDNode<"Tiny16ISD::Tiny16LDBSHI", SDTypeProfile<1, 1,
                                      [SDTCisVT<0, i16>, SDTCisVT<1, i16>]>,
                        [SDNPHasChain]>;

//===----------------------------------------------------------------------===//
// Operand Definitions.
//===----------------------------------------------------------------------===//

def MEMri : Operand<iPTR> {
  let MIOperandInfo = (ops GPR, i16imm);
  let PrintMethod = "printAddrModeMemSrc";
  let EncoderMethod = "parseMEMOperand";
}

def MEMrr : Operand<iPTR> {
  let MIOperandInfo = (ops GPR, GPR);
  let PrintMethod = "printAddrModeMemSrc";
  let EncoderMethod = "parseMEMOperand";
}

def i16imm_lo : Operand<i16>, ImmLeaf<i16, [{
  return Imm >= 0 && Imm < 256;
}]>;

def i8immSExt16 : Operand<i16>, ImmLeaf<i16, [{
  return isInt<8>(Imm);
}]>;

def i8immZExt16 : Operand<i16>, ImmLeaf<i16, [{
  return isUInt<8>(Imm);
}]>;

//===----------------------------------------------------------------------===//
// Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def ADDRrr : ComplexPattern<iPTR, 2, "SelectADDRrr", [], []>;

def ADDRri : ComplexPattern<iPTR, 2, "SelectADDRri", [], []>;
