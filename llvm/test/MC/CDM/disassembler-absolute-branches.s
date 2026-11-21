# RUN: llvm-mc -triple=cdm -filetype=obj %s -o - | llvm-objdump -d --no-show-raw-insn - | FileCheck %s

# Absolute targets are unsigned 16-bit addresses, independent of the instruction address.

.text
absolute_branches:
# CHECK-LABEL: <absolute_branches>:
.short 0x080E, 0x1234
# CHECK-NEXT: br 4660
.short 0x0801, 0x5678
# CHECK-NEXT: bne 22136
.short 0x0800, 0xFFFF
# CHECK-NEXT: beq 65535
.short 0x0004
# CHECK-NEXT: halt
