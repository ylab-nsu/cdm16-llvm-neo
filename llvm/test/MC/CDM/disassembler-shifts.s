# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d --no-print-imm-hex - | FileCheck %s

.text
shifts:             # CHECK-LABEL:  <shifts>:
.byte 0x0a, 0x10    # CHECK-NEXT:   shl     r1, r2, 1
.byte 0x53, 0x10    # CHECK-NEXT:   shl     r2, r3, 2
.byte 0x9c, 0x1a    # CHECK-NEXT:   rcl     r3, r4, 3
.byte 0xe5, 0x16    # CHECK-NEXT:   rol     r4, r5, 4
.byte 0x37, 0x15    # CHECK-NEXT:   shra    r6, fp, 5
.byte 0x78, 0x13    # CHECK-NEXT:   shr     fp, r0, 6
.byte 0x81, 0x19    # CHECK-NEXT:   ror     r0, r1, 7
.byte 0xca, 0x1d    # CHECK-NEXT:   rcr     r1, r2, 8
