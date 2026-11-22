# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d - | FileCheck %s

# NOPs are encoded as bfalse

.text
nops:                           # CHECK-LABEL: <nops>:
.byte 0xFF, 0xFF                # CHECK-NEXT:  nop
.byte 0x00, 0xFE                # CHECK-NEXT:  nop
.byte 0x00, 0xDE                # CHECK-NEXT:  nop
.byte 0xFF, 0xDF                # CHECK-NEXT:  nop
.byte 0x0F, 0x08, 0x12, 0x34    # CHECK-NEXT:  nop
.byte 0x04, 0x00                # CHECK-NEXT:  halt
.byte 0x0F, 0x08, 0x04, 0x00    # CHECK-NEXT:  nop
.byte 0x05, 0x00                # CHECK-NEXT:  wait
