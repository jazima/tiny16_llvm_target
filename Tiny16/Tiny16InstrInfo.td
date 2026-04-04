include "Tiny16InstrFormats.td"
include "Tiny16Operators.td"

//===----------------------------------------------------------------------===//
// 3 Register ALU Instructions
//===----------------------------------------------------------------------===//

class ALUOp<bits<4> opcode, string opstr, SDNode opnode>
    : Tiny16RRRInst<opcode, (outs GPR:$dst), (ins GPR:$src1, GPR:$src2),
                    !strconcat(opstr, " $dst, $src1, $src2"),
                    [(set i16:$dst, (opnode i16:$src1, i16:$src2))]> {}

def AND : ALUOp<0b0010, "and", and>;
def SHL : ALUOp<0b0100, "shl", shl>;
def SHR : ALUOp<0b0101, "shr", srl>;
def ADD : ALUOp<0b0110, "add", add>;
def SUB : ALUOp<0b0111, "sub", sub>;
def SDIV : ALUOp<0b1000, "sdiv", sdiv>;
def MUL : ALUOp<0b1001, "mul", mul>;
def SEL : Tiny16RRRInst<0b1010, (outs GPR:$dst), (ins GPR:$src1, GPR:$src2),
                        "sel $dst, $src1, $src2",
                        [(set i16:$dst, (Tiny16selc GPR:$src1, GPR:$src2))]> {}

def NOR : Tiny16RRRInst<0b0011, (outs GPR:$dst), (ins GPR:$src1, GPR:$src2),
                        "nor $dst, $src1, $src2",
                        [(set i16:$dst, (not(or i16:$src1, i16:$src2)))]> {}

//--- Patterns ---//
def : Pat<(or i16:$src1, i16:$src2), (NOR(NOR i16:$src1, i16:$src2), R0)>;
def : Pat<(xor i16:$src1, i16:$src2), (AND (NOR (NOR i16:$src1, i16:$src2), R0), (NOR(AND i16:$src1, i16:$src2), R0))>;

//===----------------------------------------------------------------------===//
// Other ALU Instructions
//===----------------------------------------------------------------------===//

def ADDI : Tiny16RIInst<0b1101, (outs GPR:$dst), (ins GPR:$src, i16imm:$imm),
                        "addi $dst, $imm",
                        [(set i16:$dst, (add i16:$src, i8immSExt16:$imm))]> {
  let Constraints = "$dst = $src";
}

def MULXSS : Tiny16RRExtRMW<0b1110, 0b0000, "mulxss $op1, $op1_in, $op2", [(set i16:$op1, (mulhs i16:$op1_in, i16:$op2))]> {}
def MULXUU : Tiny16RRExtRMW<0b1110, 0b0001, "mulxuu $op1, $op1_in, $op2", [(set i16:$op1, (mulhu i16:$op1_in, i16:$op2))]> {}
def ADDE : Tiny16RRExtRMW<0b1110, 0b1101, "adde $op1, $op1_in, $op2", []> {}
def SUBE : Tiny16RRExtRMW<0b1110, 0b1110, "sube $op1, $op1_in, $op2", []> {}
def SRA : Tiny16RRExtRMW<0b1110, 0b1111, "sra $op1, $op1_in, $op2", [(set i16:$op1, (sra i16:$op1_in, i16:$op2))]> {}
def UDIV : Tiny16RRExtRMW<0b1111, 0b0110, "udiv $op1, $op1_in, $op2", [(set i16:$op1, (udiv i16:$op1_in, i16:$op2))]> {}

//===----------------------------------------------------------------------===//
// Mov Immediate Instructions
//===----------------------------------------------------------------------===//

def MOVI : Tiny16RIInst<0b1011, (outs GPR:$dst), (ins i16imm:$imm),
                        "movi $dst, $imm", [(set i16:$dst, i16imm_lo:$imm)]> {}

def MOVHI : Tiny16RIInst<0b1100, (outs GPR:$dst), (ins GPR:$src, i16imm:$imm),
                         "movhi $dst, $imm", []> {
  let Constraints = "$dst = $src";
}

// Define movi16 as a pseudo instruction
def MOVI16
    : Tiny16PseudoInst<(outs GPR:$dst), (ins i16imm:$imm),
                       "movi16 $dst, $imm", [(set i16:$dst, imm:$imm)]> {}

// Break down MOVI16 into MOVI and MOVHI
// def : Pat<(movei16 i16imm:$imm), (MOVI i8immZExt16:$imm)> {
// }
// def : Pat<(movei16 i16imm:$imm), (MOVHI(MOVI i16imm:$imm),
//                                                   i16imm:$imm)> {
// }

// Pattern for tiny16setc. Becomes a movi16 with the value of 1, anded with RCMP
def : Pat<(Tiny16setc), (ADD R0, RCMP)>;

//===----------------------------------------------------------------------===//
// Load/Store Instructions
//===----------------------------------------------------------------------===//

def LDRR
    : InstTiny16<(outs GPR:$dst), (ins GPR:$base, GPR:$offset),
                 "ld $dst, [$base, $offset]", []> {
  bits<4> base;
  bits<4> offset;
  bits<4> dst;
  let Inst{3 -0} = 0b0000;
  let Inst{15 -12} = dst;
  let Inst{11 -8} = base;
  let Inst{7 -4} = offset;
}

def STRR : InstTiny16<(outs), (ins GPR:$val, GPR:$base, GPR:$offset),
                      "st $val, [$base, $offset]", []> {
  bits<4> base;
  bits<4> offset;
  bits<4> val;
  let Inst{3 -0} = 0b0001;
  let Inst{15 -12} = val;
  let Inst{11 -8} = base;
  let Inst{7 -4} = offset;
}

// def LDRR
//     : InstTiny16<(outs GPR:$dst), (ins MEMrr:$addr),
//                  "ld $dst, [$addr]", [(set i16:$dst, (load ADDRrr:$addr))]> {
//   bits<8> addr;
//   bits<4> dst;
//   let Inst{3 -0} = 0b0000;
//   let Inst{15 -12} = dst;
//   let Inst{11 -8} = addr{7 -4};
//   let Inst{7 -4} = addr{3 -0};
// }

// def STRR : InstTiny16<(outs), (ins GPR:$val, MEMrr:$addr),
//                       "st $val, [$addr]", [(store i16:$val, ADDRrr:$addr)]> {
//   bits<8> addr;
//   bits<4> val;
//   let Inst{3 -0} = 0b0001;
//   let Inst{15 -12} = val;
//   let Inst{11 -8} = addr{7 -4};
//   let Inst{7 -4} = addr{3 -0};
// }

// def LDBSR (load byte sign extended with 1 input regsiter)
// def STBSR (store byte sign extended with 1 input regsiter)

//--- Pseudo Instructions ---//

def LDRI : Tiny16PseudoInst<
               (outs GPR:$dst), (ins MEMri:$addr),
               "ldw $dst, [$addr]", [(set i16:$dst, (load ADDRri:$addr))]> {}

def STRI
    : Tiny16PseudoInst<(outs), (ins GPR:$val, MEMri:$addr),
                       "stw $val, [$addr]", [(store i16:$val, ADDRri:$addr)]> {}

// 8-bit loads and stored
def STBLO : Tiny16RRExtRR<0b1111, 0b0000, "stblo $op1, [$op2]", []> {}
def STBHI : Tiny16RRExtRR<0b1111, 0b0001, "stbhi $op1, [$op2]", []> {}
def LDBZLO : Tiny16RRExtInst<0b1111, 0b0010, (outs GPR:$op1), (ins GPR:$op2),
  "ldblo $op1, [$op2]", []> {}
def LDBZHI : Tiny16RRExtInst<0b1111, 0b0011, (outs GPR:$op1), (ins GPR:$op2),
  "ldbhi $op1, [$op2]", []> {}
def LDBSLO : Tiny16RRExtInst<0b1111, 0b0100, (outs GPR:$op1), (ins GPR:$op2),
  "ldbslo $op1, [$op2]", []> {}
def LDBSHI : Tiny16RRExtInst<0b1111, 0b0101, (outs GPR:$op1), (ins GPR:$op2),
  "ldbshi $op1, [$op2]", []> {}

// Pseudo instructions for zexti8 loads and stores
def STBLO_PSEUDO : Tiny16PseudoInst <
               (outs), (ins GPR:$val, MEMri:$addr),
               "stblo_pseudo $val, [$addr]", [(Tiny16STBLO GPR:$val, ADDRri:$addr)]> {}
def STBHI_PSEUDO : Tiny16PseudoInst <
               (outs), (ins GPR:$val, MEMri:$addr),
               "stbhi_pseudo $val, [$addr]", [(Tiny16STBHI GPR:$val, ADDRri:$addr)]> {}
def LDBZLO_PSEUDO : Tiny16PseudoInst <
               (outs GPR:$dst), (ins MEMri:$addr),
                "ldblo_pseudo $dst, [$addr]", [(set i16:$dst, (Tiny16LDBZLO ADDRri:$addr))]> {}
def LDBZHI_PSEUDO : Tiny16PseudoInst <
                (outs GPR:$dst), (ins MEMri:$addr),
                  "ldbhi_pseudo $dst, [$addr]", [(set i16:$dst, (Tiny16LDBZHI ADDRri:$addr))]> {}
def LDBSLO_PSEUDO : Tiny16PseudoInst <
               (outs GPR:$dst), (ins MEMri:$addr),
                "ldbslo_pseudo $dst, [$addr]", [(set i16:$dst, (Tiny16LDBSLO ADDRri:$addr))]> {}
def LDBSHI_PSEUDO : Tiny16PseudoInst <
                (outs GPR:$dst), (ins MEMri:$addr),
                  "ldbshi_pseudo $dst, [$addr]", [(set i16:$dst, (Tiny16LDBSHI ADDRri:$addr))]> {}


//--- Patterns ---//
// def : Pat<(i16(zextloadi8 ADDRri:$src)), (MOVHI(LDRI MEMri:$src), 0)>;
// def : Pat<(truncstorei8 GPR:$val, ADDRri:$addr), (STRI(MOVHI GPR:$val, 0),
//                                                      MEMri:$addr)>;

//===----------------------------------------------------------------------===//
// Comparison Instructions
//===----------------------------------------------------------------------===//

// Comparison is done by subtract instruction subtracting into R0
// We define a pseudo that expands to SUB R0, LHS, RHS
let Defs = [RCMP] in {
  // def CMP
  //     : InstTiny16<(outs), (ins GPR:$lhs, GPR:$rhs), "sub r0, $lhs, $rhs", []> {
  //   bits<4> lhs;
  //   bits<4> rhs;
  //   let Inst{3 -0} = 1100;
  //   let Inst{15 -12} = 0;
  //   let Inst{11 -8} = lhs;
  //   let Inst{7 -4} = rhs;
  // }

  def CMPEQ : Tiny16CMPInst<0b0010,
                            "cmpeq $op1, $op2", [(Tiny16cmp 0b0000, GPR:$op1, GPR:$op2)]> {
  }
  def CMPNE : Tiny16CMPInst<0b0011,
                            "cmpne $op1, $op2", [(Tiny16cmp 0b0001, GPR:$op1, GPR:$op2)]> {
  }
  def CMPLT : Tiny16CMPInst<0b0100,
                            "cmplt $op1, $op2", [(Tiny16cmp 0b0010, GPR:$op1, GPR:$op2)]> {
  }
  def CMPLE : Tiny16CMPInst<0b0101,
                            "cmple $op1, $op2", [(Tiny16cmp 0b0011, GPR:$op1, GPR:$op2)]> {
  }
  def CMPGT : Tiny16CMPInst<0b0110,
                            "cmpgt $op1, $op2", [(Tiny16cmp 0b0100, GPR:$op1, GPR:$op2)]> {
  }
  def CMPGE : Tiny16CMPInst<0b0111,
                            "cmpge $op1, $op2", [(Tiny16cmp 0b0101, GPR:$op1, GPR:$op2)]> {
  }
  def CMPULT
      : Tiny16CMPInst<0b1000,
                      "cmpult $op1, $op2", [(Tiny16cmp 0b0110, GPR:$op1, GPR:$op2)]> {}
  def CMPULE
      : Tiny16CMPInst<0b1001,
                      "cmpule $op1, $op2", [(Tiny16cmp 0b0111, GPR:$op1, GPR:$op2)]> {}
  def CMPUGT
      : Tiny16CMPInst<0b1010,
                      "cmpugt $op1, $op2", [(Tiny16cmp 0b1000, GPR:$op1, GPR:$op2)]> {}
  def CMPUGE
      : Tiny16CMPInst<0b1011,
                      "cmpuge $op1, $op2", [(Tiny16cmp 0b1001, GPR:$op1, GPR:$op2)]> {}
}

//===----------------------------------------------------------------------===//
// Return Instructions
//===----------------------------------------------------------------------===//

let isTerminator = 1, isReturn = 1, isBarrier = 1 in {
  // Return is implied by restoring PC from RA?
  // We can treat it as a pseudo that lowered to 'add pc, ra, r0' or similar if
  // needed. For now, let's assume we output "ret" which the assembler
  // understands or we expand later. But strictly, we need valid asm. PC = r15,
  // RA = r14. ADD r15, r14, r0
  def RET : Tiny16PseudoInst<(outs), (ins), "ret", [(Tiny16ret)]>;
}

//===----------------------------------------------------------------------===//
// Call Instructions
//===----------------------------------------------------------------------===//

let isCall = 1, Defs = [R0, R1, R2, R3, R4, RA] in {
  def CALL : Tiny16PseudoInst<(outs), (ins GPR:$func),
                              "call $func", [(Tiny16call i16:$func)]>;
}

def : Pattern<(i16(load_sym tglobaladdr:$addr)), [(MOVI16 $addr)]>;

//===----------------------------------------------------------------------===//
// Branch Instructions
//===----------------------------------------------------------------------===//

let isTerminator = 1, isBranch = 1, isBarrier = 1, Uses = [RCMP] in {
  def BRC
      : Tiny16RRExtRR<0b1110, 0b1100, "brc $op1, $op2", [(Tiny16brc GPR:$op1, GPR:$op2)]> {
  }
}

//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//

let Defs = [FP], Uses = [FP] in {
  def ADJCALLSTACKDOWN
      : Tiny16PseudoInst<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                         "#ADJCALLSTACKDOWN $amt1, $amt2",
                         [(callseq_start timm:$amt1, timm:$amt2)]>;
  def ADJCALLSTACKUP
      : Tiny16PseudoInst<
            (outs), (ins i16imm:$amt1, i16imm:$amt2),
            "#ADJCALLSTACKUP $amt1", [(callseq_end timm:$amt1, timm:$amt2)]>;
}