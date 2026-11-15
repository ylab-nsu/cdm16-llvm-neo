# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# Absolute JSR prints its target address.

.text
calls:
# CHECK-LABEL: <calls>:

.short 0x0008, 0x3456
# CHECK-NEXT: jsr 13398

.short 0x000B
# CHECK-NEXT: rts
