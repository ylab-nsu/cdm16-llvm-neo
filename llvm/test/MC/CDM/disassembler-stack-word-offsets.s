# RUN: llvm-mc -triple=cdm-unknown-elf -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d --no-show-raw-insn %t.o | FileCheck %s

# ADDSP scales its signed word offset to bytes.

.text
stack_word_offsets:
# CHECK-LABEL: <stack_word_offsets>:

.short 0x8A00
# CHECK-NEXT: addsp -1024

.short 0x8BFF
# CHECK-NEXT: addsp -2

.short 0x8800
# CHECK-NEXT: addsp 0

.short 0x89FF
# CHECK-NEXT: addsp 1022
