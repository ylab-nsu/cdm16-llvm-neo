# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d - | FileCheck %s

.text
absolute_branches:      # CHECK-LABEL:  <absolute_branches>:
.short 0x080E, 0x1234   # CHECK-NEXT:   br  0x1234
.short 0x0801, 0x5678   # CHECK-NEXT:   bne 0x5678
.short 0x0800, 0xFFFF   # CHECK-NEXT:   beq 0xffff
.short 0x080B, 0xDEAD   # CHECK-NEXT:   blt 0xdead
.short 0x080C, 0xBEEF   # CHECK-NEXT:   bgt 0xbeef
.short 0x0008, 0x3456   # CHECK-NEXT:   jsr 0x3456
.short 0x0008, 0x0000   # CHECK-NEXT:   jsr 0x0
.short 0x0008, 0xFFFF   # CHECK-NEXT:   jsr 0xffff
.short 0x0004           # CHECK-NEXT:   halt
