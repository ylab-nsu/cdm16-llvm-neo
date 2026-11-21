# RUN: llvm-mc -triple=cdm -filetype=obj %s -o - | llvm-objdump -d --no-show-raw-insn - | FileCheck %s

# The encoded shift amount is one less than the printed amount.

.text
shifts:
# CHECK-LABEL: <shifts>:
.short 0x100A
# CHECK-NEXT: shl r1, r2, 1
.short 0x11CA
# CHECK-NEXT: shl r1, r2, 8
.short 0x1DCA
# CHECK-NEXT: rcr r1, r2, 8
