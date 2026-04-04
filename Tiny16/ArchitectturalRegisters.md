## High Level

| Encoding | Encoding Dec | Name                | Description                                                                                                                                                                                                  |
| -------- | ------------ | ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 4'b0000  | 0            | r0                  | Zero register (always stores 0)                                                                                                                                                                              |
| 4'b0001  | 1            | r1                  | General purpose register 1                                                                                                                                                                                   |
| 4'b0010  | 2            | r2                  | General purpose register 2                                                                                                                                                                                   |
| 4'b0011  | 3            | r3                  | General purpose register 3                                                                                                                                                                                   |
| 4'b0100  | 4            | r4                  | General purpose register 4                                                                                                                                                                                   |
| 4'b0101  | 5            | r5                  | General purpose register 5                                                                                                                                                                                   |
| 4'b0110  | 6            | r6                  | General purpose register 6                                                                                                                                                                                   |
| 4'b0111  | 7            | r7                  | General purpose register 7                                                                                                                                                                                   |
| 4'b1000  | 8            | r8                  | General purpose register 8                                                                                                                                                                                   |
| 4'b1001  | 9            | r9                  | General purpose register 9                                                                                                                                                                                   |
| 4'b1010  | 10           | rR                  | Reserved register for internal use                                                                                                                                                                           |
| 4'b1011  | 11           | rCmp (Compare)      | Multi Purpose (Comparison Bits, Overflow, Div by Zero)<br>a. Store comparison results<br>b. Store carry bit<br>c. Store exception information<br>d. Store Interrupt information (one hot interrupt location) |
| 4'b1100  | 12           | fp (frame pointer)  | Points to the last frame                                                                                                                                                                                     |
| 4'b1101  | 13           | sp (stack pointer)  | Points to the top of the stack                                                                                                                                                                               |
| 4'b1110  | 14           | ra (return address) | Stores the return address for a subroutine                                                                                                                                                                   |
| 4'b1111  | 15           | PC                  | Stores the program counter (Memory location of program)                                                                                                                                                      |

## Register Specifics
#### r0
Zero register (always 0)

#### r1
General purpose register 1
Return register 1
Argument Register 1
Caller saved

#### r2
General purpose register 2
Return register 2
Argument Register 2
Caller saved general purpose register

#### r3
General purpose register 3
Return register 3
Argument Register 3
Caller saved general purpose register

#### r4
General purpose register 4
Return register 4
Argument Register 4
Caller saved general purpose register

#### r5
General purpose register 4
Callee saved general purpose register

#### r6
General purpose register 4
Callee saved general purpose register

#### r7
General purpose register 4
Callee saved general purpose register

#### r8
General purpose register 4
Callee saved general purpose register

#### r9
General purpose register 4
Callee saved general purpose register

#### rR
Special reserved register when using personal assembler. Used to enable higher level instructions.
Can be used by other assemblers or assemblers as a general purpose register (Callee saved).

#### rCmp
Special register
Only certain bits are writable (but entire register is readable)
Stores interrupt enables (writable and readable), exceptions (readable and clearable), and comparison information from certain ALU operations

| 15               | 14          | 13          | 12          | 11          | 10          | 9           | 8           |
| ---------------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
| Enable Interrupt | Interrupt 0 | Interrupt 1 | Interrupt 2 | Interrupt 3 | Interrupt 4 | interrupt 5 | Interrupt 6 |

| 7         | 6                               | 5                        | 4                     | 3               | 2                     | 1            | 0    |
| --------- | ------------------------------- | ------------------------ | --------------------- | --------------- | --------------------- | ------------ | ---- |
| Exception | Exception (Invalid mem access?) | Divide by zero exception | Overflow or Underflow | Carry or Borrow | Greater Than Unsigned | Greater Than | Zero |
#### fp
Frame pointer

#### sp
Stack pointer

#### ra
Reserved for storing the return address

#### pc
program counter


