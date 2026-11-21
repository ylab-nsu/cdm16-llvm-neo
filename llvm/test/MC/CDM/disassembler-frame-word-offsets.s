# RUN: llvm-mc -triple=cdm -filetype=obj %s -o - | llvm-objdump -d --no-show-raw-insn - | FileCheck %s

# Frame word offsets are printed in bytes, including the negative opcode forms.

.text
frame_word_offsets:
# CHECK-LABEL: <frame_word_offsets>:
.short 0x6201
# CHECK-NEXT: lsw r1, -128
.short 0x63F9
# CHECK-NEXT: lsw r1, -2
.short 0x6001
# CHECK-NEXT: lsw r1, 0
.short 0x61F9
# CHECK-NEXT: lsw r1, 126

.short 0x6E07
# CHECK-NEXT: ssw fp, -128
.short 0x6FFF
# CHECK-NEXT: ssw fp, -2
.short 0x6C07
# CHECK-NEXT: ssw fp, 0
.short 0x6DFF
# CHECK-NEXT: ssw fp, 126
