# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# Alternating lengths must leave the decoder at the next instruction boundary.

.text
mixed_lengths:
# CHECK-LABEL: <mixed_lengths>:

.short 0x0004
# CHECK-NEXT: 0: halt

.short 0x2010, 0x0000
# CHECK-NEXT: 2: ldi r0, 0

.short 0x2005
# CHECK-NEXT: 6: push r5

.short 0x080E, 0x1234
# CHECK-NEXT: 8: br 4660

.short 0x200A
# CHECK-NEXT: c: pop r2

.short 0x0005
# CHECK-NEXT: e: wait

.short 0x0004
# CHECK-NEXT: 10: halt
