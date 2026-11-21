# RUN: llvm-mc -triple=cdm -filetype=obj %s -o - | llvm-objdump -d --no-show-raw-insn - | FileCheck %s

# Absolute JSR prints its target address.

.text
calls:
# CHECK-LABEL: <calls>:
.short 0x0008, 0x3456
# CHECK-NEXT: jsr 13398
.short 0x0008, 0x0000
# CHECK-NEXT: jsr 0
.short 0x0008, 0xFFFF
# CHECK-NEXT: jsr 65535
.short 0x000B
# CHECK-NEXT: rts
