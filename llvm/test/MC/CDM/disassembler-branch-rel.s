# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d - | FileCheck %s

.text
relative_branches:  # CHECK-LABEL:              <relative_branches>:
.byte 0x00, 0x00    # CHECK-NEXT:  0: 00 00     zero
.byte 0x00, 0x00    # CHECK-NEXT:  2: 00 00     zero
.byte 0x00, 0x00    # CHECK-NEXT:  4: 00 00     zero
.byte 0xfe, 0xdd    # CHECK-NEXT:  6: fe dd     br   0x4
.byte 0xfd, 0xd9    # CHECK-NEXT:  8: fd d9     bgt  0x4
.byte 0xfc, 0xd7    # CHECK-NEXT:  a: fc d7     blt  0x4
.byte 0xfb, 0xd1    # CHECK-NEXT:  c: fb d1     bhi  0x4
.byte 0xfa, 0xc7    # CHECK-NEXT:  e: fa c7     blo  0x4
.byte 0xf9, 0x8f    # CHECK-NEXT: 10: f9 8f     jsr  0x4
.byte 0x00, 0x00    # CHECK-NEXT: 12: 00 00     zero
.byte 0xf5, 0xdd    # CHECK-NEXT: 14: f5 dd     br   0x0
.byte 0xf4, 0xd9    # CHECK-NEXT: 16: f4 d9     bgt  0x0
.byte 0xf3, 0xd7    # CHECK-NEXT: 18: f3 d7     blt  0x0
.byte 0xf2, 0xd1    # CHECK-NEXT: 1a: f2 d1     bhi  0x0
.byte 0xf1, 0xc7    # CHECK-NEXT: 1c: f1 c7     blo  0x0
.byte 0xf0, 0x8f    # CHECK-NEXT: 1e: f0 8f     jsr  0x0
.byte 0x00, 0x00    # CHECK-NEXT: 20: 00 00     zero
.byte 0x0d, 0xfc    # CHECK-NEXT: 22: 0d fc     br   0x3e
.byte 0x0c, 0xf8    # CHECK-NEXT: 24: 0c f8     bgt  0x3e
.byte 0x0b, 0xf6    # CHECK-NEXT: 26: 0b f6     blt  0x3e
.byte 0x0a, 0xf0    # CHECK-NEXT: 28: 0a f0     bhi  0x3e
.byte 0x09, 0xe6    # CHECK-NEXT: 2a: 09 e6     blo  0x3e
.byte 0x08, 0x8c    # CHECK-NEXT: 2c: 08 8c     jsr  0x3e
.byte 0x00, 0x00    # CHECK-NEXT: 2e: 00 00     zero
.byte 0x08, 0xfc    # CHECK-NEXT: 30: 08 fc     br   0x42
.byte 0x07, 0xf8    # CHECK-NEXT: 32: 07 f8     bgt  0x42
.byte 0x06, 0xf6    # CHECK-NEXT: 34: 06 f6     blt  0x42
.byte 0x05, 0xf0    # CHECK-NEXT: 36: 05 f0     bhi  0x42
.byte 0x04, 0xe6    # CHECK-NEXT: 38: 04 e6     blo  0x42
.byte 0x03, 0x8c    # CHECK-NEXT: 3a: 03 8c     jsr  0x42
.byte 0x00, 0x00    # CHECK-NEXT: 3c: 00 00     zero
.byte 0x00, 0x00    # CHECK-NEXT: 3e: 00 00     zero
.byte 0x00, 0x00    # CHECK-NEXT: 40: 00 00     zero
.byte 0x04, 0x00    # CHECK-NEXT: 42: 04 00     halt
