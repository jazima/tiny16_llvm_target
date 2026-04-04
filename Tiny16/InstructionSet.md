## High Level

| Encoding | Encoding Hex | Encoding Dec | Name   | Instruction     | Description                                            |
| -------- | ------------ | ------------ | ------ | --------------- | ------------------------------------------------------ |
| 4'b0000  | 0            | 0            | ld     | ld rC rA rB     | Load into r1 from memory location r2+r3                |
| 4'b0001  | 1            | 1            | st     | st rC rA rB     | Store r1 into memory location r2+r3                    |
| 4'b0010  | 2            | 2            | movi   | movi rC IMM8    | Move value IMM8 into lower 8 bits of r1                |
| 4'b0011  | 3            | 3            | movhi  | movhi rC IMM8   | Move value IMM9 into upper 8 bits of r1                |
| 4'b0100  | 4            | 4            | bz     | bz rC (IMM8)    | Branch if zero to r1 + IMM8                            |
| 4'b0101  | 5            | 5            | bgt    | bgt rC (IMM8)   | Branch if greater than to r1 + IMM8                    |
| 4'b0110  | 6            | 6            | bgtu   | bgtu rC (IMM8)  | Branch if greater than unsigned to r1 + IMM8           |
| 4'b0111  | 7            | 7            | and    | and rC rA rB    | And r2 and r3 and store the result into r1             |
| 4'b1000  | 8            | 8            | nor    | nor rC rA rB    | Nor r2 and r3 and store the result into r1             |
| 4'b1001  | 9            | 9            | rol    | rol rC rA rB    | Rotate left r2 by r3 and store the result into r1      |
| 4'b1010  | a            | 10           | add    | add rC rA rB    | Add r2 and r3 and store the result into r1             |
| 4'b1011  | b            | 11           | addi   | addi rC IMM8    |                                                        |
| 4'b1100  | c            | 12           | sub    | sub rC rA rB    | Subtract r2 and r3 and store the result into r1        |
| 4'b1101  | d            | 13           | div    | div rC rA rB    | Divide r2/r3 and store the result into r1              |
| 4'b1110  | e            | 14           | mul    | mul rC rA rB    | Multiply r2 and r3 and store the lower 16 bits into r1 |
| 4'b1111  | f            | 15           | mulxss | mulxss rC rA rB | Multiply r2 and r3 and store the upper 16 bits into r1 |
## Instruction Specifics
## Load (ld)

| Encoding | Encoding Dec | Name | Instruction |
| -------- | ------------ | ---- | ----------- |
| 4'b0000  | 0            | ld   | ld rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 0   | 0   | 0   | 0   |
rC ← Mem16\[rA + rB]
Loads the value in memory from address rA+rB and stores it in rC

## Store (st)

| Encoding | Encoding Dec | Name | Instruction |
| -------- | ------------ | ---- | ----------- |
| 4'b0001  | 1            | st   | st rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 0   | 0   | 0   | 1   |
Mem16\[rA + rB] ← rC
Store value in rC to memory location rA+rB

## Move Immediate (movi)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b0010  | 2            | movi | movi rC IMM8 |

| 15  | 14  | 13  | 12  | 11   | 10  | 9   | 8   |
| --- | --- | --- | --- | ---- | --- | --- | --- |
| rC  |     |     |     | IMM8 |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
|     |     |     |     | 0   | 0   | 1   | 0   |
rC ← IMM8 (Retains upper 8 bits of reg)
Store IMM8 in lower 8 bits of rC

## Move Immediate Hi (movhi)

| Encoding | Encoding Dec | Name  | Instruction   |
| -------- | ------------ | ----- | ------------- |
| 4'b0011  | 3            | movhi | movhi rC IMM8 |

| 15  | 14  | 13  | 12  | 11   | 10  | 9   | 8   |
| --- | --- | --- | --- | ---- | --- | --- | --- |
| rC  |     |     |     | IMM8 |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
|     |     |     |     | 0   | 0   | 1   | 1   |
rC ← IMM8 (Retains lower 8 bits of reg)
Store IMM8 in upper 8 bits of rC

## Branch if Zero (bz)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b0100  | 4            | bz   | bz rC (IMM8) |

| 15  | 14  | 13  | 12  | 11   | 10  | 9   | 8   |
| --- | --- | --- | --- | ---- | --- | --- | --- |
| rC  |     |     |     | IMM8 |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
|     |     |     |     | 0   | 1   | 0   | 0   |
if (rCmp[z] == 0b1)
then PC ← rC + SIGNED(IMM8)

## Branch if Greater Than

| Encoding | Encoding Dec | Name | Instruction    |
| -------- | ------------ | ---- | -------------- |
| 4'b0101  | 5            | bgt  | brgt rC (IMM8) |

| 15  | 14  | 13  | 12  | 11   | 10  | 9   | 8   |
| --- | --- | --- | --- | ---- | --- | --- | --- |
| rC  |     |     |     | IMM8 |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
|     |     |     |     | 0   | 1   | 0   | 1   |
if (rCmp[gt] == 0b1)
then PC ← rC + SIGNED(IMM8)

## Branch if Greater Than Unsigned

| Encoding | Encoding Dec | Name | Instruction    |
| -------- | ------------ | ---- | -------------- |
| 4'b0110  | 6            | bgtu | bgtu rC (IMM8) |

| 15  | 14  | 13  | 12  | 11   | 10  | 9   | 8   |
| --- | --- | --- | --- | ---- | --- | --- | --- |
| rC  |     |     |     | IMM8 |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
|     |     |     |     | 0   | 1   | 1   | 0   |
if (rCmp[gt] == 0b1)
then PC ← rC + SIGNED(IMM8)

## And (and)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b0111  | 7            | and  | and rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 0   | 1   | 1   | 1   |
rC ← rA & rB

## Not Or (nor)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b1000  | 8            | nor  | nor rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 1   | 0   | 0   | 0   |
rC ← ~(rA | rB)

## Rol (rol)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b1001  | 9            | rol  | rol rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 1   | 0   | 0   | 1   |
rC ← rA rotated left rB[3:0] positions

## Add (add)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b1010  | 10           | add  | add rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 0   | 1   | 0   | 0   |
rC ← rA + rB

## Add Immediate (addi)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b1011  | 11           | addi | addi rC IMM8 |

| 15  | 14  | 13  | 12  | 11   | 10  | 9   | 8   |
| --- | --- | --- | --- | ---- | --- | --- | --- |
| rC  |     |     |     | IMM8 |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
|     |     |     |     | 1   | 0   | 1   | 01  |
rC ← rC + SIGNEX(IMM8)

## Subtract (sub)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b1100  | 12           | sub  | sub rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 1   | 1   | 0   | 0   |
rC ← rA - rB

## Divide (div)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b1101  | 13           | div  | div rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 1   | 1   | 0   | 1   |
rC ← SIGNED(rA) //SIGNED(rB)

## Multiply (mul)

| Encoding | Encoding Dec | Name | Instruction  |
| -------- | ------------ | ---- | ------------ |
| 4'b1110  | 14           | mul  | mul rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 1   | 1   | 1   | 0   |
rC ← (rA * rB)[15:0]

## Multiply High Signed (mulxss)

| Encoding | Encoding Dec | Name   | Instruction     |
| -------- | ------------ | ------ | --------------- |
| 4'b1111  | 15           | mulxss | mulxss rC rA rB |

| 15  | 14  | 13  | 12  | 11  | 10  | 9   | 8   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rC  |     |     |     | rA  |     |     |     |

| 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- |
| rB  |     |     |     | 1   | 1   | 1   | 1   |
rC ← (SIGNED(rA) * SIGNED(rB))[31:16]