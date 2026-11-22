# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d --no-print-imm-hex - | FileCheck %s

.text
frame_word_offsets: # CHECK-LABEL: <frame_word_offsets>:
.byte 0x01, 0x62    # CHECK-NEXT:   lsw   r1, -128
.byte 0xF9, 0x63    # CHECK-NEXT:   lsw   r1, -2
.byte 0x01, 0x60    # CHECK-NEXT:   lsw   r1, 0
.byte 0xF9, 0x61    # CHECK-NEXT:   lsw   r1, 126

.byte 0x07, 0x6E    # CHECK-NEXT:   ssw   fp, -128
.byte 0xFF, 0x6F    # CHECK-NEXT:   ssw   fp, -2
.byte 0x07, 0x6C    # CHECK-NEXT:   ssw   fp, 0
.byte 0xFF, 0x6D    # CHECK-NEXT:   ssw   fp, 126

frame_byte_offsets: # CHECK-LABEL: <frame_byte_offsets>:
.byte 0x01, 0x66    # CHECK-NEXT:   lsb   r1, -64
.byte 0xF9, 0x67    # CHECK-NEXT:   lsb   r1, -1
.byte 0x01, 0x64    # CHECK-NEXT:   lsb   r1, 0
.byte 0xF9, 0x65    # CHECK-NEXT:   lsb   r1, 63

.byte 0x01, 0x6A    # CHECK-NEXT:   lssb  r1, -64
.byte 0xF9, 0x6B    # CHECK-NEXT:   lssb  r1, -1
.byte 0x01, 0x68    # CHECK-NEXT:   lssb  r1, 0
.byte 0xF9, 0x69    # CHECK-NEXT:   lssb  r1, 63

.byte 0x01, 0x72    # CHECK-NEXT:   ssb   r1, -64
.byte 0xF9, 0x73    # CHECK-NEXT:   ssb   r1, -1
.byte 0x01, 0x70    # CHECK-NEXT:   ssb   r1, 0
.byte 0xF9, 0x71    # CHECK-NEXT:   ssb   r1, 63
