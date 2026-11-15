# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn --mattr=+m,+e %t.o | FileCheck %s

.text
.globl test_instructions
test_instructions:

# === Instructions without operands ===

# CHECK-LABEL: <test_instructions>:

.short 0x0000
# CHECK: zero

.short 0x0004
# CHECK-NEXT: halt

.short 0x0005
# CHECK-NEXT: wait

.short 0x0006
# CHECK-NEXT: ei

.short 0x0007
# CHECK-NEXT: di

.short 0x0009
# CHECK-NEXT: rti

.short 0x000A
# CHECK-NEXT: pupc

.short 0x000B
# CHECK-NEXT: rts

.short 0x000C
# CHECK-NEXT: pusp

.short 0x000D
# CHECK-NEXT: posp

.short 0x000E
# CHECK-NEXT: pups

.short 0x000F
# CHECK-NEXT: pops


# === One-register instructions ===

.short 0x2002
# CHECK-NEXT: push r2

.short 0x200A
# CHECK-NEXT: pop r2

.short 0x201A
# CHECK-NEXT: jsrr r2

.short 0x2022
# CHECK-NEXT: ldsp r2

.short 0x202A
# CHECK-NEXT: stsp r2

.short 0x2032
# CHECK-NEXT: ldps r2

.short 0x203A
# CHECK-NEXT: stps r2

.short 0x2042
# CHECK-NEXT: ldpc r2

.short 0x204A
# CHECK-NEXT: stpc r2

.short 0x2052
# CHECK-NEXT: addsp r2

# Extension E
.short 0x2072
# CHECK-NEXT: ldssp r2

.short 0x207A
# CHECK-NEXT: stssp r2


# === Two-register instructions ===

.short 0x400A
# CHECK-NEXT: move r1, r2

# Extension E
.short 0x408A
# CHECK-NEXT: swpw r1, r2

.short 0x40CA
# CHECK-NEXT: swpb r1, r2

# Extension M
.short 0x430A
# CHECK-NEXT: umul r1, r2

.short 0x434A
# CHECK-NEXT: smul r1, r2

.short 0x438A
# CHECK-NEXT: udiv r1, r2

.short 0x43CA
# CHECK-NEXT: sdiv r1, r2


# === Memory: register addressing ===

.short 0x500A
# CHECK-NEXT: ldw r1, r2

.short 0x504A
# CHECK-NEXT: ldb r1, r2

.short 0x508A
# CHECK-NEXT: ldsb r1, r2

.short 0x50CA
# CHECK-NEXT: lcw r1, r2

.short 0x510A
# CHECK-NEXT: lcb r1, r2

.short 0x514A
# CHECK-NEXT: lcsb r1, r2

.short 0x518A
# CHECK-NEXT: stw r1, r2

.short 0x51CA
# CHECK-NEXT: stb r1, r2


# === Memory: register + register ===

.short 0xA05A
# CHECK-NEXT: ldw r3, r1, r2

.short 0xA25A
# CHECK-NEXT: ldb r3, r1, r2

.short 0xA45A
# CHECK-NEXT: ldsb r3, r1, r2

.short 0xA65A
# CHECK-NEXT: lcw r3, r1, r2

.short 0xA85A
# CHECK-NEXT: lcb r3, r1, r2

.short 0xAA5A
# CHECK-NEXT: lcsb r3, r1, r2

.short 0xAC5A
# CHECK-NEXT: stw r3, r1, r2

.short 0xAE5A
# CHECK-NEXT: stb r3, r1, r2


# === Two-register ALU ===

.short 0x580A
# CHECK-NEXT: neg r1, r2

.short 0x584A
# CHECK-NEXT: not r1, r2

.short 0x588A
# CHECK-NEXT: sxt r1, r2

.short 0x58CA
# CHECK-NEXT: scl r1, r2


# === Three-register ALU ===

.short 0xB05A
# CHECK-NEXT: and r3, r1, r2

.short 0xB25A
# CHECK-NEXT: or r3, r1, r2

.short 0xB45A
# CHECK-NEXT: xor r3, r1, r2

.short 0xB65A
# CHECK-NEXT: bic r3, r1, r2

.short 0xB85A
# CHECK-NEXT: add r3, r1, r2

.short 0xBA5A
# CHECK-NEXT: addc r3, r1, r2

.short 0xBC5A
# CHECK-NEXT: sub r3, r1, r2

.short 0xBE5A
# CHECK-NEXT: subc r3, r1, r2


# === Non-destructive ALU ===

.short 0x480A
# CHECK-NEXT: bit r1, r2

.short 0x498A
# CHECK-NEXT: cmp r1, r2
