//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
class InstTiny16<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  field bits<16> Inst;

  let Namespace = "Tiny16";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
  let Size = 2;
}

// Tiny16 pseudo instructions format
class Tiny16PseudoInst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTiny16<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
}

//===----------------------------------------------------------------------===//
// Register / register / register instruction:
//===----------------------------------------------------------------------===//
class Tiny16RRRInst<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTiny16<outs, ins, asmstr, pattern> {
    bits<4> dst;
    bits<4> src1;
    bits<4> src2;
    let Inst{3-0} = opcode;
    let Inst{15-12} = dst;
    let Inst{11-8} = src1;
    let Inst{7-4} = src2;
}

//===----------------------------------------------------------------------===//
// Register / Immediate instruction:
// opcode = 4 bits.
//===----------------------------------------------------------------------===//
class Tiny16RIInst<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTiny16<outs, ins, asmstr, pattern> {
    bits<4> dst;
    bits<8> imm;
    let Inst{3-0} = opcode;
    let Inst{15-12} = dst;
    let Inst{11-4} = imm;
}

//===----------------------------------------------------------------------===//
// Register / Register extended instruction:
// base opcode = 4 bits.
// extended opcode = 4 bits.
//===----------------------------------------------------------------------===//
class Tiny16RRExtInst<bits<4> opcode, bits<4> ext_opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTiny16<outs, ins, asmstr, pattern> {
    bits<4> op1;
    bits<4> op2;
    let Inst{3-0} = opcode;
    let Inst{7-4} = ext_opcode;
    let Inst{15-12} = op1;
    let Inst{11-8} = op2;
}

//===----------------------------------------------------------------------===//
// Register / Register extended instruction for writing back into op1
//===----------------------------------------------------------------------===//
class Tiny16RRExtRMW<bits<4> opcode, bits<4> ext_opcode, string asmstr, list<dag> pattern>
    : Tiny16RRExtInst<opcode, ext_opcode, 
                     (outs GPR:$op1), (ins GPR:$op1_in, GPR:$op2),
                     asmstr, pattern> {
  // This tells LLVM that the output $op1 must be the same register as the input $op1_in
  let Constraints = "$op1 = $op1_in";
}

class Tiny16RRExtRR<bits<4> opcode, bits<4> ext_opcode, string asmstr, list<dag> pattern>
    : Tiny16RRExtInst<opcode, ext_opcode, 
                     (outs ), (ins GPR:$op1, GPR:$op2),
                     asmstr, pattern> {
}

//===----------------------------------------------------------------------===//
// CMP Instruction
//===----------------------------------------------------------------------===//

class Tiny16CMPInst<bits<4> cmp_op, string asmstr, list<dag> pattern>
    : Tiny16RRExtInst<0b1110, cmp_op, (outs ), (ins GPR:$op1, GPR:$op2),
                     asmstr, pattern> {
}