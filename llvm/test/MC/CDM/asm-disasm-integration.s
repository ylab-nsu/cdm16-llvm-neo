# RUN: llvm-mc -triple=cdm -filetype=obj %s -o - | llvm-objdump -d --no-show-raw-insn - | FileCheck %s

# Integration coverage for the parser, emitter, relaxation and disassembler.

.text
integration:
# CHECK-LABEL: <integration>:
halt
# CHECK-NEXT: halt
wait
# CHECK-NEXT: wait
push r0
# CHECK-NEXT: push r0
push r5
# CHECK-NEXT: push r5
pop r2
# CHECK-NEXT: pop r2
add r0, r1, r2
# CHECK-NEXT: add r0, r1, r2
add r3, r4, r5
# CHECK-NEXT: add r3, r4, r5
ldi r0, 1000
# CHECK-NEXT: ldi r0, 1000
ldi r3, 1234
# CHECK-NEXT: ldi r3, 1234
jsr 1000
# CHECK-NEXT: jsr 1000
br 2000
# CHECK-NEXT: br 2000
beq 1000
# CHECK-NEXT: beq 1000
bne 2000
# CHECK-NEXT: bne 2000
bgt 3000
# CHECK-NEXT: bgt 3000
bhs 1000
# CHECK-NEXT: bhs 1000
blo 1000
# CHECK-NEXT: blo 1000
bmi 1000
# CHECK-NEXT: bmi 1000
bpl 1000
# CHECK-NEXT: bpl 1000
bge 1000
# CHECK-NEXT: bge 1000
blt 1000
# CHECK-NEXT: blt 1000
ble 1000
# CHECK-NEXT: ble 1000
