# RUN: llvm-mc -triple=cdm -filetype=obj %s -o - | llvm-objdump -d --no-show-raw-insn - | FileCheck %s

.text
imm16:	                # CHECK-LABEL:  <imm16>:
.short 0x2010, 0x8000	# CHECK-NEXT:   ldi r0, 0x8000
.short 0x2010, 0xFFFF	# CHECK-NEXT:   ldi r0, 0xffff
.short 0x2010, 0x0000	# CHECK-NEXT:   ldi r0, 0x0
.short 0x2010, 0x7FFF	# CHECK-NEXT:   ldi r0, 0x7fff
.short 0x0004           # CHECK-NEXT:   halt
