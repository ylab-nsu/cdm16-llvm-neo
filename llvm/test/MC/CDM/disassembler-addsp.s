# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d --no-print-imm-hex - | FileCheck %s

.text
stack_word_offsets: # CHECK-LABEL: <stack_word_offsets>:
.byte 0x00, 0x8A    # CHECK-NEXT:  addsp -1024
.byte 0xFF, 0x8B    # CHECK-NEXT:  addsp -2
.byte 0x00, 0x88    # CHECK-NEXT:  addsp 0
.byte 0xFF, 0x89    # CHECK-NEXT:  addsp 1022
