# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# Frame byte offsets must not be scaled like word offsets.

.text
frame_byte_offsets:
# CHECK-LABEL: <frame_byte_offsets>:

.short 0x6601
# CHECK-NEXT: lsb r1, -64

.short 0x67F9
# CHECK-NEXT: lsb r1, -1

.short 0x6401
# CHECK-NEXT: lsb r1, 0

.short 0x65F9
# CHECK-NEXT: lsb r1, 63

.short 0x6A01
# CHECK-NEXT: lssb r1, -64

.short 0x6BF9
# CHECK-NEXT: lssb r1, -1

.short 0x6801
# CHECK-NEXT: lssb r1, 0

.short 0x69F9
# CHECK-NEXT: lssb r1, 63

.short 0x7201
# CHECK-NEXT: ssb r1, -64

.short 0x73F9
# CHECK-NEXT: ssb r1, -1

.short 0x7001
# CHECK-NEXT: ssb r1, 0

.short 0x71F9
# CHECK-NEXT: ssb r1, 63
