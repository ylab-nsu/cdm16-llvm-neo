# RUN: llvm-mc -triple=cdm -filetype=obj -o - %s | llvm-objdump -d --stop-address=26 --no-print-imm-hex - | FileCheck %s

# LDI selects the short form when possible, branches select their relative forms.
# LDI with large constants uses an additional word, branches with distant targets use their absolute forms.

.text
relaxation: # CHECK-LABEL:              <relaxation>:
ldi r0, 63  # CHECK-NEXT:  f8 75        ldi r0, 63
ldi r1, -64 # CHECK-NEXT:  01 76        ldi r1, -64
ldi r2, 64  # CHECK-NEXT:  12 20 40 00  ldi r2, 64
ldi r3, -65 # CHECK-NEXT:  13 20 bf ff  ldi r3, 65471
jsr far     # CHECK-NEXT:  08 00 00 10  jsr 0x1000
br  far     # CHECK-NEXT:  0e 08 00 10  br 0x1000
bne far     # CHECK-NEXT:  01 08 00 10  bne 0x1000
halt        # CHECK-NEXT:  04 00        halt

.org 4096
far:
halt
