# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d - | FileCheck %s

.text
mixed_lengths:          # CHECK-LABEL:  <mixed_lengths>:
.short 0x0004           # CHECK-NEXT:   halt
.short 0x2010, 0x0000   # CHECK-NEXT:   ldi r0, 0x0
.short 0x2005           # CHECK-NEXT:   push r5
.short 0x080E, 0x1234   # CHECK-NEXT:   br 0x1234
.short 0x200A           # CHECK-NEXT:   pop r2
.short 0x0005           # CHECK-NEXT:   wait
.short 0x0004           # CHECK-NEXT:   halt
