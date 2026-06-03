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
# Initialize stack
ldi fp, __stack_start
stsp fp

# Copy .data
ldi r0, __data_rom
ldi r1, __data
ldi r2, __data_length
cmp r2, 0
br 1f
0:
lcw r0, r3
stw r1, r3
add r0, 2
add r1, 2
add r2, -2
1:
bnz 0b

# Initialize .bss
ldi r0, __bss
ldi r1, __bss_length
ldi r2, 0
cmp r1, 0
br 1f
0:
stw r0, r2
add r0, 2
add r1, -2
1:
bnz 0b

# Hand control over to user code
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
