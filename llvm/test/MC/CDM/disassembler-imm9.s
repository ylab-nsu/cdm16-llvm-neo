# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# PUSH uses a nine-bit field and a separate negative opcode.

.text
imm9:
# CHECK-LABEL: <imm9>:

.short 0x8600
# CHECK-NEXT: push -512

.short 0x87FF
# CHECK-NEXT: push -1

.short 0x8400
# CHECK-NEXT: push 0

.short 0x85FF
# CHECK-NEXT: push 511
