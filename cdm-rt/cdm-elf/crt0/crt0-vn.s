.section .ivt.reset_vector, "a", @progbits
.short _start, 0

.section .ivt.exceptions, "a", @progbits
.short _unaligned_sp, 1
.short _unaligned_pc, 2
.short _invalid_inst, 3
.short _double_fault, 4

.section .text._start, "ax", @progbits
.global _start
.type _start,%function
_start:
ldi fp, 0
stsp fp
jsr main
halt

.global _default_handler
.type _default_handler,%function
_default_handler:
ldi r0, 0xDED0
ldps r1
or r0, r1, r0
pop r1
halt
