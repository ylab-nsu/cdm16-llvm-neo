.section .ivt.reset_vector, "a", @progbits
.short _start, 0

.section .ivt.exceptions, "a", @progbits
.short _ex_unaligned_sp, 0x1
.short _ex_unaligned_pc, 0x2
.short _ex_invalid_inst, 0x3
.short _ex_double_fault, 0x4
.short _ex_priv_violation, 0x5
.short _ex_reserved_6, 0x6
.short _ex_syscall, 0x7
.short _ex_reserved_8, 0x8
.short _ex_reserved_9, 0x9
.short _ex_reserved_a, 0xa
.short _ex_reserved_b, 0xb
.short _ex_reserved_c, 0xc
.short _ex_reserved_d, 0xd
.short _ex_reserved_e, 0xe
.short _ex_reserved_f, 0xf

.section .text._start, "ax", @progbits
.global _start
.type _start,%function
_start:
ldi fp, __stack_start
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
