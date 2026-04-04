// Tiny16RegisterInfo.td

class Tiny16Reg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "Tiny16";
}

// CPU registers
def R0   : Tiny16Reg<0, "r0">;  // zero register
def R1   : Tiny16Reg<1, "r1">;
def R2   : Tiny16Reg<2, "r2">;
def R3   : Tiny16Reg<3, "r3">;
def R4   : Tiny16Reg<4, "r4">; 
def R5   : Tiny16Reg<5, "r5">;
def R6   : Tiny16Reg<6, "r6">;
def R7   : Tiny16Reg<7, "r7">;
def R8   : Tiny16Reg<8, "r8">; 
def R9   : Tiny16Reg<9, "r9">;
def R10   : Tiny16Reg<10, "r10">;
def RR   : Tiny16Reg<11, "rRR">;
def RCMP : Tiny16Reg<12, "rCmp">; 
def FP   : Tiny16Reg<13, "fp">;
def RA   : Tiny16Reg<14, "ra">;
def PC   : Tiny16Reg<15, "pc">;

// General-purpose register class (excluding PC, rCmp by default)
def GPR : RegisterClass<"Tiny16", [i16], 16,
                        (add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10,
                             FP, RR, RA)>;