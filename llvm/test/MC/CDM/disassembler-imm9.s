# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d --no-print-imm-hex - | FileCheck %s

.text
imm9:               # CHECK-LABEL: <imm9>:
.byte 0x00, 0x86	# CHECK-NEXT:  push -512
.byte 0xFF, 0x87	# CHECK-NEXT:  push -1
.byte 0x00, 0x84	# CHECK-NEXT:  push 0
.byte 0xFF, 0x85	# CHECK-NEXT:  push 511
