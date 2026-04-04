//===----------------------------------------------------------------------===//
// This describes the calling conventions for Tiny16 architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Tiny16 Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_Tiny16 : CallingConv<[
    // Promote i8 values to i16
    CCIfType<[i8], CCPromoteToType<i16>>,

    // i16 are returned in registers R1, R2, R3, R4, R5
    CCIfType<[i16], CCAssignToReg<[R1, R2, R3, R4, R5]>>,
    CCIfType<[i32], CCAssignToReg<[R1, R2, R3, R4, R5]>>,

    // Integer values get stored in stack slots that are 2 bytes in
    // size and 2-byte aligned
    CCIfType<[i16], CCAssignToStack<2, 2>>,
    CCIfType<[i32], CCAssignToStack<4, 2>>
]>;

//===----------------------------------------------------------------------===//
// Tiny16 Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_Tiny16 : CallingConv<[
    // Promote i8 arguments to i16.
    CCIfType<[i8], CCPromoteToType<i16>>,

    // The first 5 integer arguments are passed in integer registers.
    CCIfType<[i16], CCAssignToReg<[R1, R2, R3, R4, R5]>>,
    CCIfType<[i32], CCAssignToReg<[R1, R2, R3, R4, R5]>>,

    // Integer values get stored in stack slots that are 4 bytes in
    // size and 4-byte aligned.
    CCIfType<[i16], CCAssignToStack<2, 2>>,
    CCIfType<[i32], CCAssignToStack<4, 2>>
]>;

def CSR : CalleeSavedRegs<(add R6, R7, R8, R9, R10)>;
