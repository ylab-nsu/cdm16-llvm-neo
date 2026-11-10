
# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --mattr=+m,+e %t.o | FileCheck %s

.text
.globl test_immediates
test_immediates:

# CHECK-LABEL: <test_immediates>:

# Signed 7-bit ADD
.short 0x7A00
.short 0x7BF8
.short 0x7808
.short 0x79F8

# CHECK: add{{[[:space:]]+}}r0, -64
# CHECK-NEXT: dec{{[[:space:]]+}}r0
# CHECK-NEXT: inc{{[[:space:]]+}}r0
# CHECK-NEXT: add{{[[:space:]]+}}r0, 63

# Signed 7-bit CMP
.short 0x7E01
.short 0x7DF9

# CHECK: cmp{{[[:space:]]+}}r1, -64
# CHECK-NEXT: cmp{{[[:space:]]+}}r1, 63

# Short LDI
.short 0x7602
.short 0x75FA

# CHECK: ldi{{[[:space:]]+}}r2, -64
# CHECK-NEXT: ldi{{[[:space:]]+}}r2, 63

# FP-relative word offsets
.short 0x6201
.short 0x61F9
.short 0x6E01
.short 0x6DF9

# CHECK: lsw{{[[:space:]]+}}r1, -128
# CHECK-NEXT: lsw{{[[:space:]]+}}r1, 126
# CHECK-NEXT: ssw{{[[:space:]]+}}r1, -128
# CHECK-NEXT: ssw{{[[:space:]]+}}r1, 126

# FP-relative byte offsets
.short 0x6601
.short 0x65F9
.short 0x6A01
.short 0x69F9
.short 0x7201
.short 0x71F9

# CHECK: lsb{{[[:space:]]+}}r1, -64
# CHECK-NEXT: lsb{{[[:space:]]+}}r1, 63
# CHECK-NEXT: lssb{{[[:space:]]+}}r1, -64
# CHECK-NEXT: lssb{{[[:space:]]+}}r1, 63
# CHECK-NEXT: ssb{{[[:space:]]+}}r1, -64
# CHECK-NEXT: ssb{{[[:space:]]+}}r1, 63

# Signed 10-bit immediates
.short 0x8600
.short 0x85FF
.short 0x8A00
.short 0x89FF
.short 0x8E00
.short 0x8DFF

# CHECK: push{{[[:space:]]+}}-512
# CHECK-NEXT: push{{[[:space:]]+}}511
# CHECK-NEXT: addsp{{[[:space:]]+}}-1024
# CHECK-NEXT: addsp{{[[:space:]]+}}1022
# CHECK-NEXT: jsr{{[[:space:]]+}}-512
# CHECK-NEXT: jsr{{[[:space:]]+}}511

# Signed 16-bit immediates
.short 0x2010
.short 0x8000
.short 0x2010
.short 0x7FFF

# CHECK: ldi{{[[:space:]]+}}r0, -32768
# CHECK-NEXT: ldi{{[[:space:]]+}}r0, 32767

# Shift amount is encoded as amount - 1
.short 0x100A
.short 0x11CA
.short 0x1DCA

# CHECK: shl{{[[:space:]]+}}r1, r2, 1
# CHECK-NEXT: shl{{[[:space:]]+}}r1, r2, 8
# CHECK-NEXT: rcr{{[[:space:]]+}}r1, r2, 8

.short 0xFFFF

# CHECK: nop
