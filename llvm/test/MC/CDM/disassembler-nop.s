# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# All FALSE branches print as NOP; an absolute FALSE branch still consumes four bytes.

.text
false_branches:
# CHECK-LABEL: <false_branches>:

.short 0xFFFF
# CHECK-NEXT: nop

.short 0xFE00
# CHECK-NEXT: nop

.short 0xDE00
# CHECK-NEXT: nop

.short 0xDFFF
# CHECK-NEXT: nop

.short 0x080F, 0x1234
# CHECK-NEXT: nop

.short 0x0004
# CHECK-NEXT: halt

.short 0x080F, 0x0004
# CHECK-NEXT: nop

.short 0x0005
# CHECK-NEXT: wait
