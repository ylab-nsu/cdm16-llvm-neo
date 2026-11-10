
# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --mattr=+m,+e %t.o | FileCheck %s

.text
.globl test_branches
test_branches:

# Positive conditional branch: +1 word
.short 0xE201
# CHECK-LABEL: <test_branches>:
# CHECK: bne{{[[:space:]]+}}4

# Negative conditional branch: -2 words
.short 0xC3FE
# CHECK-NEXT: bne{{[[:space:]]+}}0

# Equal: +1 word
.short 0xE001
# CHECK-NEXT: beq{{[[:space:]]+}}8

# Equal: -1 word (branch to itself)
.short 0xC1FF
# CHECK-NEXT: beq{{[[:space:]]+}}6

# Greater than: +1 word
.short 0xF801
# CHECK-NEXT: bgt{{[[:space:]]+}}12

# Less than: -3 words
.short 0xD7FD
# CHECK-NEXT: blt{{[[:space:]]+}}6

# Maximum positive displacement: +511 words
.short 0xEBFF
# CHECK-NEXT: bpl{{[[:space:]]+}}1036

# Minimum negative displacement: -512 words
# Target wraps in 16-bit address space
.short 0xC200
# CHECK-NEXT: bne{{[[:space:]]+}}64528

# Unconditional branch, zero displacement
.short 0xFC00
# CHECK-NEXT: br{{[[:space:]]+}}18

# Unconditional branch, -512 words
.short 0xDC00
# CHECK-NEXT: br{{[[:space:]]+}}64532

# Special NOP encoding
.short 0xFFFF
# CHECK-NEXT: nop

# 16-bit relative JSR
.short 0x8C01
.short 0x8FFF
.short 0x8E00
.short 0x8DFF

# CHECK-NEXT: jsr{{[[:space:]]+}}1
# CHECK-NEXT: jsr{{[[:space:]]+}}-1
# CHECK-NEXT: jsr{{[[:space:]]+}}-512
# CHECK-NEXT: jsr{{[[:space:]]+}}511

# 32-bit absolute JSR
.short 0x0008
.short 0x3456
# CHECK-NEXT: jsr{{[[:space:]]+}}13398

# 32-bit absolute BR
.short 0x080E
.short 0x1234
# CHECK-NEXT: br{{[[:space:]]+}}4660

# 32-bit absolute BNE
.short 0x0801
.short 0x5678
# CHECK-NEXT: bne{{[[:space:]]+}}22136

.short 0x000B
# CHECK-NEXT: rts
