# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d --no-print-imm-hex - | FileCheck %s

.text
imm6:               # CHECK-LABEL:  <imm6>:
.byte 0x00, 0x7A	# CHECK-NEXT:   add r0, -64
.byte 0xF8, 0x7B	# CHECK-NEXT:   dec r0
.byte 0x08, 0x78	# CHECK-NEXT:   inc r0
.byte 0xF8, 0x79	# CHECK-NEXT:   add r0, 63

.byte 0x01, 0x7E	# CHECK-NEXT:   cmp r1, -64
.byte 0xF9, 0x7F	# CHECK-NEXT:   cmp r1, -1
.byte 0x01, 0x7C	# CHECK-NEXT:   cmp r1, 0
.byte 0xF9, 0x7D	# CHECK-NEXT:   cmp r1, 63

.byte 0x02, 0x76	# CHECK-NEXT:   ldi r2, -64
.byte 0xFA, 0x77	# CHECK-NEXT:   ldi r2, -1
.byte 0x02, 0x74	# CHECK-NEXT:   ldi r2, 0
.byte 0xFA, 0x75	# CHECK-NEXT:   ldi r2, 63

.byte 0x07, 0x7A	# CHECK-NEXT:   add fp, -64
