# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d --no-print-imm-hex - | FileCheck %s

.text
integration:    # CHECK-LABEL: <integration>:
halt            # CHECK-NEXT:  halt
wait            # CHECK-NEXT:  wait
push r0         # CHECK-NEXT:  push r0
push r5         # CHECK-NEXT:  push r5
pop r2          # CHECK-NEXT:  pop  r2
add r0, r1, r2  # CHECK-NEXT:  add  r0, r1, r2
add r3, r4, r5  # CHECK-NEXT:  add  r3, r4, r5
ldi r0, 1000    # CHECK-NEXT:  ldi  r0, 1000
ldi r3, 1234    # CHECK-NEXT:  ldi  r3, 1234
jsr 1000        # CHECK-NEXT:  jsr  0x3e8
br 2000         # CHECK-NEXT:  br   0x7d0
beq 1000        # CHECK-NEXT:  beq  0x3e8
bne 2000        # CHECK-NEXT:  bne  0x7d0
bgt 3000        # CHECK-NEXT:  bgt  0xbb8
bhs 1000        # CHECK-NEXT:  bhs  0x3e8
blo 1000        # CHECK-NEXT:  blo  0x3e8
bmi 1000        # CHECK-NEXT:  bmi  0x3e8
bpl 1000        # CHECK-NEXT:  bpl  0x3e8
bge 1000        # CHECK-NEXT:  bge  0x3e8
blt 1000        # CHECK-NEXT:  blt  0x3e8
ble 1000        # CHECK-NEXT:  ble  0x3e8
