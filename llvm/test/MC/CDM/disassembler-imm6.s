# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# The sign of these immediates is encoded in the opcode, outside the six-bit field.

.text
imm6:
# CHECK-LABEL: <imm6>:

.short 0x7A00
# CHECK-NEXT: add r0, -64

.short 0x7BF8
# CHECK-NEXT: dec r0

.short 0x7808
# CHECK-NEXT: inc r0

.short 0x79F8
# CHECK-NEXT: add r0, 63

.short 0x7E01
# CHECK-NEXT: cmp r1, -64

.short 0x7FF9
# CHECK-NEXT: cmp r1, -1

.short 0x7C01
# CHECK-NEXT: cmp r1, 0

.short 0x7DF9
# CHECK-NEXT: cmp r1, 63

.short 0x7602
# CHECK-NEXT: ldi r2, -64

.short 0x77FA
# CHECK-NEXT: ldi r2, -1

.short 0x7402
# CHECK-NEXT: ldi r2, 0

.short 0x75FA
# CHECK-NEXT: ldi r2, 63

.short 0x7A07
# CHECK-NEXT: add fp, -64
