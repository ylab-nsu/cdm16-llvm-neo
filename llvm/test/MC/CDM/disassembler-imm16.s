# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# Long LDI must decode even when its value also fits the short form.

.text
imm16:
# CHECK-LABEL: <imm16>:

.short 0x2010, 0x8000
# CHECK-NEXT: ldi r0, -32768

.short 0x2010, 0xFFFF
# CHECK-NEXT: ldi r0, -1

.short 0x2010, 0x0000
# CHECK-NEXT: ldi r0, 0

.short 0x2010, 0x7FFF
# CHECK-NEXT: ldi r0, 32767

.short 0x0004
# CHECK-NEXT: halt
