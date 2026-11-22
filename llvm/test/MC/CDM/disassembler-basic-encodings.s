# RUN: llvm-mc -triple=cdm --mattr=+m,+e -filetype=obj -o - %s | llvm-objdump -d --mattr=+m,+e - | FileCheck %s

.text

inst_0op:           # CHECK-LABEL: <inst_0op>:
.byte 0x00, 0x00    # CHECK-NEXT:  zero
.byte 0x04, 0x00    # CHECK-NEXT:  halt
.byte 0x05, 0x00    # CHECK-NEXT:  wait
.byte 0x06, 0x00    # CHECK-NEXT:  ei
.byte 0x07, 0x00    # CHECK-NEXT:  di
.byte 0x09, 0x00    # CHECK-NEXT:  rti
.byte 0x0A, 0x00    # CHECK-NEXT:  pupc
.byte 0x0B, 0x00    # CHECK-NEXT:  rts
.byte 0x0C, 0x00    # CHECK-NEXT:  pusp
.byte 0x0D, 0x00    # CHECK-NEXT:  posp
.byte 0x0E, 0x00    # CHECK-NEXT:  pups
.byte 0x0F, 0x00    # CHECK-NEXT:  pops

inst_1op:           # CHECK-LABEL:  <inst_1op>:
.byte 0x02, 0x20    # CHECK-NEXT:   push    r2
.byte 0x0A, 0x20    # CHECK-NEXT:   pop     r2
.byte 0x1A, 0x20    # CHECK-NEXT:   jsrr    r2
.byte 0x22, 0x20    # CHECK-NEXT:   ldsp    r2
.byte 0x2A, 0x20    # CHECK-NEXT:   stsp    r2
.byte 0x32, 0x20    # CHECK-NEXT:   ldps    r2
.byte 0x3A, 0x20    # CHECK-NEXT:   stps    r2
.byte 0x42, 0x20    # CHECK-NEXT:   ldpc    r2
.byte 0x4A, 0x20    # CHECK-NEXT:   stpc    r2
.byte 0x52, 0x20    # CHECK-NEXT:   addsp   r2
.byte 0x72, 0x20    # CHECK-NEXT:   ldssp   r2
.byte 0x7A, 0x20    # CHECK-NEXT:   stssp   r2

inst_2op:           # CHECK-LABEL:  <inst_2op>:
.byte 0x0A, 0x40    # CHECK-NEXT:   move    r1, r2
.byte 0x8A, 0x40    # CHECK-NEXT:   swpw    r1, r2
.byte 0xCA, 0x40    # CHECK-NEXT:   swpb    r1, r2
.byte 0x0A, 0x43    # CHECK-NEXT:   umul    r1, r2
.byte 0x4A, 0x43    # CHECK-NEXT:   smul    r1, r2
.byte 0x8A, 0x43    # CHECK-NEXT:   udiv    r1, r2
.byte 0xCA, 0x43    # CHECK-NEXT:   sdiv    r1, r2

inst_mem2:          # CHECK-LABEL:  <inst_mem2>:
.byte 0x0A, 0x50    # CHECK-NEXT:   ldw     r1, r2
.byte 0x4A, 0x50    # CHECK-NEXT:   ldb     r1, r2
.byte 0x8A, 0x50    # CHECK-NEXT:   ldsb    r1, r2
.byte 0xCA, 0x50    # CHECK-NEXT:   lcw     r1, r2
.byte 0x0A, 0x51    # CHECK-NEXT:   lcb     r1, r2
.byte 0x4A, 0x51    # CHECK-NEXT:   lcsb    r1, r2
.byte 0x8A, 0x51    # CHECK-NEXT:   stw     r1, r2
.byte 0xCA, 0x51    # CHECK-NEXT:   stb     r1, r2

inst_mem3:          # CHECK-LABEL:  <inst_mem3>:
.byte 0x5A, 0xA0    # CHECK-NEXT:   ldw     r3, r1, r2
.byte 0x5A, 0xA2    # CHECK-NEXT:   ldb     r3, r1, r2
.byte 0x5A, 0xA4    # CHECK-NEXT:   ldsb    r3, r1, r2
.byte 0x5A, 0xA6    # CHECK-NEXT:   lcw     r3, r1, r2
.byte 0x5A, 0xA8    # CHECK-NEXT:   lcb     r3, r1, r2
.byte 0x5A, 0xAA    # CHECK-NEXT:   lcsb    r3, r1, r2
.byte 0x5A, 0xAC    # CHECK-NEXT:   stw     r3, r1, r2
.byte 0x5A, 0xAE    # CHECK-NEXT:   stb     r3, r1, r2

inst_alu2:          # CHECK-LABEL:  <inst_alu2>:
.byte 0x0A, 0x58    # CHECK-NEXT:   neg     r1, r2
.byte 0x4A, 0x58    # CHECK-NEXT:   not     r1, r2
.byte 0x8A, 0x58    # CHECK-NEXT:   sxt     r1, r2
.byte 0xCA, 0x58    # CHECK-NEXT:   scl     r1, r2

inst_alu3:          # CHECK-LABEL:  <inst_alu3>:
.byte 0x5A, 0xB0    # CHECK-NEXT:   and     r3, r1, r2
.byte 0x5A, 0xB2    # CHECK-NEXT:   or      r3, r1, r2
.byte 0x5A, 0xB4    # CHECK-NEXT:   xor     r3, r1, r2
.byte 0x5A, 0xB6    # CHECK-NEXT:   bic     r3, r1, r2
.byte 0x5A, 0xB8    # CHECK-NEXT:   add     r3, r1, r2
.byte 0x5A, 0xBA    # CHECK-NEXT:   addc    r3, r1, r2
.byte 0x5A, 0xBC    # CHECK-NEXT:   sub     r3, r1, r2
.byte 0x5A, 0xBE    # CHECK-NEXT:   subc    r3, r1, r2

inst_alu_ind:       # CHECK-LABEL:  <inst_alu_ind>:
.byte 0x0A, 0x48    # CHECK-NEXT:   bit     r1, r2
.byte 0x8A, 0x49    # CHECK-NEXT:   cmp     r1, r2
