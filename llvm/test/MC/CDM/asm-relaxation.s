# RUN: llvm-mc -triple=cdm -filetype=obj %s -o - | llvm-objdump -d --stop-address=26 - | FileCheck %s

# LDI selects the short form when possible; distant branch targets use absolute forms.
.text
relaxation:
# CHECK-LABEL: <relaxation>:
ldi r0, 63
# CHECK-NEXT: 0: f8 75 ldi r0, 63
ldi r1, -64
# CHECK-NEXT: 2: 01 76 ldi r1, -64
ldi r2, 64
# CHECK-NEXT: 4: 12 20 40 00 ldi r2, 64
ldi r3, -65
# CHECK-NEXT: 8: 13 20 bf ff ldi r3, -65
jsr far
# CHECK-NEXT: c: 08 00 00 10 jsr 4096
br far
# CHECK-NEXT: 10: 0e 08 00 10 br 4096
bne far
# CHECK-NEXT: 14: 01 08 00 10 bne 4096
halt
# CHECK-NEXT: 18: 04 00 halt

.org 4096
far:
halt
