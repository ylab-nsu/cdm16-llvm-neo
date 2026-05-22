#include "cdm_asm.inc"

RSECT(stdc_leading_zeros_uc)
GLOBAL(stdc_leading_zeros_uc)
move r0, r1
bnz stdc_leading_zeros_uc_beg
ldi r0, 8
rts
stdc_leading_zeros_uc_beg:
clr r0
cmp r1, 0x10
bhs stdc_leading_zeros_uc_1
add r0, 4
shl r1, r1, 4
stdc_leading_zeros_uc_1:
ldi r2, 0x40
cmp r1, r2
bhs stdc_leading_zeros_uc_2
add r0, 2
shl r1, r1, 2
stdc_leading_zeros_uc_2:
ldi r2, 0x80
cmp r1, r2
bhs stdc_leading_zeros_uc_end
add r0, 1
stdc_leading_zeros_uc_end:
rts

RSECT(stdc_leading_zeros_ui)
GLOBAL(stdc_leading_zeros_ui)
GLOBAL(stdc_leading_zeros_us)
move r0, r1
bnz stdc_leading_zeros_us_beg
ldi r0, 16
rts
stdc_leading_zeros_us_beg:
clr r0
ldi r2, 0x0100
cmp r1, r2
bhs stdc_leading_zeros_us_1
add r0, 8
shl r1, r1, 8
stdc_leading_zeros_us_1:
ldi r2, 0x1000
cmp r1, r2
bhs stdc_leading_zeros_us_2
add r0, 4
shl r1, r1, 4
stdc_leading_zeros_us_2:
ldi r2, 0x4000
cmp r1, r2
bhs stdc_leading_zeros_us_3
add r0, 2
shl r1, r1, 2
stdc_leading_zeros_us_3:
ldi r2, 0x8000
cmp r1, r2
bhs stdc_leading_zeros_us_end
add r0, 1
stdc_leading_zeros_us_end:
rts

RSECT(stdc_leading_zeros_ul)
EXTERN(stdc_leading_zeros_us)
GLOBAL(stdc_leading_zeros_ul)
cmp r1, 0
bne stdc_leading_zeros_ul_high_test
jsr stdc_leading_zeros_us
add r0, 16
rts
stdc_leading_zeros_ul_high_test:
move r1, r0
jsr stdc_leading_zeros_us
rts

RSECT(stdc_leading_zeros_ull)
EXTERN(stdc_leading_zeros_us)
GLOBAL(stdc_leading_zeros_ull)
cmp r3, 0
bne stdc_leading_zeros_ull_4_test
cmp r2, 0
bne stdc_leading_zeros_ull_3_test
cmp r1, 0
bne stdc_leading_zeros_ull_2_test
jsr stdc_leading_zeros_us
add r0, 48
rts

stdc_leading_zeros_ull_2_test:
move r1, r0
jsr stdc_leading_zeros_us
add r0, 32
rts
stdc_leading_zeros_ull_3_test:
move r2, r0
jsr stdc_leading_zeros_us
add r0, 16
rts
stdc_leading_zeros_ull_4_test:
move r3, r0
jsr stdc_leading_zeros_us
rts

RSECT(stdc_trailing_zeros_uc)
GLOBAL(stdc_trailing_zeros_uc)
move r0, r1
bnz stdc_trailing_zeros_uc_beg
ldi r0, 8
rts
stdc_trailing_zeros_uc_beg:
clr r0
ldi r2, 0x0F
bit r1, r2
bnz stdc_trailing_zeros_uc_1
add r0, 4
shr r1, r1, 4
stdc_trailing_zeros_uc_1:
ldi r2, 0x03
bit r1, r2
bnz stdc_trailing_zeros_uc_2
add r0, 2
shr r1, r1, 2
stdc_trailing_zeros_uc_2:
ldi r2, 0x01
bit r1, r2
bnz stdc_trailing_zeros_uc_end
add r0, 1
stdc_trailing_zeros_uc_end:
rts

RSECT(stdc_trailing_zeros_ui)
GLOBAL(stdc_trailing_zeros_us)
GLOBAL(stdc_trailing_zeros_ui)
move r0, r1
bnz stdc_trailing_zeros_us_beg
ldi r0, 16
rts
stdc_trailing_zeros_us_beg:
clr r0
ldi r2, 0x00FF
bit r1, r2
bnz stdc_trailing_zeros_us_1
add r0, 8
shr r1, r1, 8
stdc_trailing_zeros_us_1:
ldi r2, 0x000F
bit r1, r2
bnz stdc_trailing_zeros_us_2
add r0, 4
shr r1, r1, 4
stdc_trailing_zeros_us_2:
ldi r2, 0x0003
bit r1, r2
bnz stdc_trailing_zeros_us_3
add r0, 2
shr r1, r1, 2
stdc_trailing_zeros_us_3:
ldi r2, 0x0001
bit r1, r2
bnz stdc_trailing_zeros_us_end
add r0, 1
stdc_trailing_zeros_us_end:
rts

RSECT(stdc_trailing_zeros_ul)
EXTERN(stdc_trailing_zeros_us)
GLOBAL(stdc_trailing_zeros_ul)
cmp r0, 0
bne stdc_trailing_zeros_ul_low_test
move r1, r0
jsr stdc_trailing_zeros_us
add r0, 16
rts
stdc_trailing_zeros_ul_low_test:
jsr stdc_trailing_zeros_us
rts

RSECT(stdc_trailing_zeros_ull)
EXTERN(stdc_trailing_zeros_us)
GLOBAL(stdc_trailing_zeros_ull)
cmp r0, 0
bne stdc_trailing_zeros_ull_0_test
cmp r1, 0
bne stdc_trailing_zeros_ull_1_test
cmp r2, 0
bne stdc_trailing_zeros_ull_2_test
move r3, r0
jsr stdc_trailing_zeros_us
add r0, 48
rts

stdc_trailing_zeros_ull_2_test:
move r2, r0
jsr stdc_trailing_zeros_us
add r0, 32
rts
stdc_trailing_zeros_ull_1_test:
move r1, r0
jsr stdc_trailing_zeros_us
add r0, 16
rts
stdc_trailing_zeros_ull_0_test:
jsr stdc_trailing_zeros_us
rts

RSECT(stdc_leading_ones_uc)
EXTERN(stdc_leading_zeros_uc)
GLOBAL(stdc_leading_ones_uc)
not r0
scl r0, r0
jsr stdc_leading_zeros_uc
rts

RSECT(stdc_leading_ones_ui)
EXTERN(stdc_leading_zeros_us)
GLOBAL(stdc_leading_ones_us)
GLOBAL(stdc_leading_ones_ui)
not r0
jsr stdc_leading_zeros_us
rts

RSECT(stdc_leading_ones_ul)
EXTERN(stdc_leading_zeros_ul)
GLOBAL(stdc_leading_ones_ul)
not r0
not r1
jsr stdc_leading_zeros_ul
rts

RSECT(stdc_leading_ones_ull)
EXTERN(stdc_leading_zeros_ull)
GLOBAL(stdc_leading_ones_ull)
not r0
not r1
not r2
not r3
jsr stdc_leading_zeros_ull
rts

RSECT(stdc_trailing_ones_uc)
EXTERN(stdc_trailing_zeros_uc)
GLOBAL(stdc_trailing_ones_uc)
not r0
scl r0, r0
jsr stdc_trailing_zeros_uc
rts

RSECT(stdc_trailing_ones_ui)
EXTERN(stdc_trailing_zeros_us)
GLOBAL(stdc_trailing_ones_us)
GLOBAL(stdc_trailing_ones_ui)
not r0
jsr stdc_trailing_zeros_us
rts

RSECT(stdc_trailing_ones_ul)
EXTERN(stdc_trailing_zeros_ul)
GLOBAL(stdc_trailing_ones_ul)
not r0
not r1
jsr stdc_trailing_zeros_ul
rts

RSECT(stdc_trailing_ones_ull)
EXTERN(stdc_trailing_zeros_ull)
GLOBAL(stdc_trailing_ones_ull)
not r0
not r1
not r2
not r3
jsr stdc_trailing_zeros_ull
rts

RSECT(stdc_first_leading_one_uc)
GLOBAL(stdc_first_leading_one_uc)
move r0, r1
bnz stdc_first_leading_one_uc_beg
ldi r0, 0
rts
stdc_first_leading_one_uc_beg:
clr r0
cmp r1, 0x10
bhs stdc_first_leading_one_uc_1
add r0, 4
shl r1, r1, 4
stdc_first_leading_one_uc_1:
ldi r2, 0x40
cmp r1, r2
bhs stdc_first_leading_one_uc_2
add r0, 2
shl r1, r1, 2
stdc_first_leading_one_uc_2:
ldi r2, 0x80
cmp r1, r2
bhs stdc_first_leading_one_uc_end
add r0, 1
stdc_first_leading_one_uc_end:
inc r0
rts

RSECT(stdc_first_leading_one_ui)
GLOBAL(stdc_first_leading_one_us)
GLOBAL(stdc_first_leading_one_ui)
move r0, r1
bnz stdc_first_leading_one_us_beg
ldi r0, 0
rts
stdc_first_leading_one_us_beg:
clr r0
ldi r2, 0x0100
cmp r1, r2
bhs stdc_first_leading_one_us_1
add r0, 8
shl r1, r1, 8
stdc_first_leading_one_us_1:
ldi r2, 0x1000
cmp r1, r2
bhs stdc_first_leading_one_us_2
add r0, 4
shl r1, r1, 4
stdc_first_leading_one_us_2:
ldi r2, 0x4000
cmp r1, r2
bhs stdc_first_leading_one_us_3
add r0, 2
shl r1, r1, 2
stdc_first_leading_one_us_3:
ldi r2, 0x8000
cmp r1, r2
bhs stdc_first_leading_one_us_end
add r0, 1
stdc_first_leading_one_us_end:
inc r0
rts

RSECT(stdc_first_leading_one_ul)
EXTERN(stdc_first_leading_one_us)
GLOBAL(stdc_first_leading_one_ul)
cmp r1, 0
bne stdc_first_leading_one_ul_high_test
jsr stdc_first_leading_one_us
cmp r0, 0
beq stdc_first_leading_one_ul_end
add r0, 16
rts
stdc_first_leading_one_ul_high_test:
move r1, r0
jsr stdc_first_leading_one_us
stdc_first_leading_one_ul_end:
rts

RSECT(stdc_first_leading_one_ull)
EXTERN(stdc_first_leading_one_us)
GLOBAL(stdc_first_leading_one_ull)
cmp r3, 0
bne stdc_first_leading_one_ull_4_test
cmp r2, 0
bne stdc_first_leading_one_ull_3_test
cmp r1, 0
bne stdc_first_leading_one_ull_2_test
jsr stdc_first_leading_one_us
cmp r0, 0
beq stdc_first_leading_one_ull_end
add r0, 48
rts

stdc_first_leading_one_ull_2_test:
move r1, r0
jsr stdc_first_leading_one_us
add r0, 32
rts
stdc_first_leading_one_ull_3_test:
move r2, r0
jsr stdc_first_leading_one_us
add r0, 16
rts
stdc_first_leading_one_ull_4_test:
move r3, r0
jsr stdc_first_leading_one_us
stdc_first_leading_one_ull_end:
rts

RSECT(stdc_first_trailing_one_uc)
GLOBAL(stdc_first_trailing_one_uc)
move r0, r1
bnz stdc_first_trailing_one_uc_beg
ldi r0, 0
rts
stdc_first_trailing_one_uc_beg:
clr r0
ldi r2, 0x0F
bit r1, r2
bnz stdc_first_trailing_one_uc_1
add r0, 4
shr r1, r1, 4
stdc_first_trailing_one_uc_1:
ldi r2, 0x03
bit r1, r2
bnz stdc_first_trailing_one_uc_2
add r0, 2
shr r1, r1, 2
stdc_first_trailing_one_uc_2:
ldi r2, 0x01
bit r1, r2
bnz stdc_first_trailing_one_uc_end
add r0, 1
stdc_first_trailing_one_uc_end:
inc r0
rts

RSECT(stdc_first_trailing_one_ui)
GLOBAL(stdc_first_trailing_one_us)
GLOBAL(stdc_first_trailing_one_ui)
move r0, r1
bnz stdc_first_trailing_one_us_beg
ldi r0, 0
rts
stdc_first_trailing_one_us_beg:
clr r0
ldi r2, 0x00FF
bit r1, r2
bnz stdc_first_trailing_one_us_1
add r0, 8
shr r1, r1, 8
stdc_first_trailing_one_us_1:
ldi r2, 0x000F
bit r1, r2
bnz stdc_first_trailing_one_us_2
add r0, 4
shr r1, r1, 4
stdc_first_trailing_one_us_2:
ldi r2, 0x0003
bit r1, r2
bnz stdc_first_trailing_one_us_3
add r0, 2
shr r1, r1, 2
stdc_first_trailing_one_us_3:
ldi r2, 0x0001
bit r1, r2
bnz stdc_first_trailing_one_us_end
add r0, 1
stdc_first_trailing_one_us_end:
inc r0
rts

RSECT(stdc_first_trailing_one_ul)
EXTERN(stdc_first_trailing_one_us)
GLOBAL(stdc_first_trailing_one_ul)
cmp r0, 0
bne stdc_first_trailing_one_ul_low_test
move r1, r0
jsr stdc_first_trailing_one_us
cmp r0, 0
beq stdc_first_trailing_one_ul_end
add r0, 16
rts
stdc_first_trailing_one_ul_low_test:
jsr stdc_first_trailing_one_us
stdc_first_trailing_one_ul_end:
rts

RSECT(stdc_first_trailing_one_ull)
EXTERN(stdc_first_trailing_one_us)
GLOBAL(stdc_first_trailing_one_ull)
cmp r0, 0
bne stdc_first_trailing_one_ull_0_test
cmp r1, 0
bne stdc_first_trailing_one_ull_1_test
cmp r2, 0
bne stdc_first_trailing_one_ull_2_test
move r3, r0
jsr stdc_first_trailing_one_us
cmp r0, 0
beq stdc_first_trailing_one_ull_end
add r0, 48
rts

stdc_first_trailing_one_ull_2_test:
move r2, r0
jsr stdc_first_trailing_one_us
add r0, 32
rts
stdc_first_trailing_one_ull_1_test:
move r1, r0
jsr stdc_first_trailing_one_us
add r0, 16
rts
stdc_first_trailing_one_ull_0_test:
jsr stdc_first_trailing_one_us
stdc_first_trailing_one_ull_end:
rts

RSECT(stdc_first_leading_zero_uc)
EXTERN(stdc_first_leading_one_uc)
GLOBAL(stdc_first_leading_zero_uc)
not r0
scl r0, r0
jsr stdc_first_leading_one_uc
rts

RSECT(stdc_first_leading_zero_ui)
EXTERN(stdc_first_leading_one_us)
GLOBAL(stdc_first_leading_zero_us)
GLOBAL(stdc_first_leading_zero_ui)
not r0
jsr stdc_first_leading_one_us
rts

RSECT(stdc_first_leading_zero_ul)
EXTERN(stdc_first_leading_one_ul)
GLOBAL(stdc_first_leading_zero_ul)
not r0
not r1
jsr stdc_first_leading_one_ul
rts

RSECT(stdc_first_leading_zero_ull)
EXTERN(stdc_first_leading_one_ull)
GLOBAL(stdc_first_leading_zero_ull)
not r0
not r1
not r2
not r3
jsr stdc_first_leading_one_ull
rts


RSECT(stdc_first_trailing_zero_uc)
EXTERN(stdc_first_trailing_one_uc)
GLOBAL(stdc_first_trailing_zero_uc)
not r0
scl r0, r0
jsr stdc_first_trailing_one_uc
rts

RSECT(stdc_first_trailing_zero_ui)
EXTERN(stdc_first_trailing_one_us)
GLOBAL(stdc_first_trailing_zero_us)
GLOBAL(stdc_first_trailing_zero_ui)
not r0
jsr stdc_first_trailing_one_us
rts

RSECT(stdc_first_trailing_zero_ul)
EXTERN(stdc_first_trailing_one_ul)
GLOBAL(stdc_first_trailing_zero_ul)
not r0
not r1
jsr stdc_first_trailing_one_ul
rts

RSECT(stdc_first_trailing_zero_ull)
EXTERN(stdc_first_trailing_one_ull)
GLOBAL(stdc_first_trailing_zero_ull)
not r0
not r1
not r2
not r3
jsr stdc_first_trailing_one_ull
rts

RSECT(stdc_count_ones_uc)
GLOBAL(stdc_count_ones_uc)
move r0, r1
shr r1, r1, 1
ldi r2, 0x55
and r1, r2, r1
sub r0, r1, r0

move r0, r1
shr r1, r1, 2
ldi r2, 0x33
and r0, r2, r0
and r1, r2, r1
add r0, r1, r0

move r0, r1
shr r1, r1, 4
ldi r2, 0x0F
and r0, r2, r0
and r1, r2, r1
add r0, r1, r0
rts

RSECT(stdc_count_ones_ui)
GLOBAL(stdc_count_ones_us)
GLOBAL(stdc_count_ones_ui)
move r0, r1
shr r1, r1, 1
ldi r2, 0x5555
and r1, r2, r1
sub r0, r1, r0

move r0, r1
shr r1, r1, 2
ldi r2, 0x3333
and r0, r2, r0
and r1, r2, r1
add r0, r1, r0

move r0, r1
shr r1, r1, 4
ldi r2, 0x0F0F
and r0, r2, r0
and r1, r2, r1
add r0, r1, r0

move r0, r1
shr r1, r1, 8
ldi r2, 0x00FF
and r0, r2, r0
and r1, r2, r1
add r0, r1, r0
rts

RSECT(stdc_count_ones_ul)
EXTERN(stdc_count_ones_us)
GLOBAL(stdc_count_ones_ul)
push r1
jsr stdc_count_ones_us
move r0, r1
pop r0
push r1
jsr stdc_count_ones_us
pop r1
add r1, r0, r0
rts

RSECT(stdc_count_ones_ull)
EXTERN(stdc_count_ones_us)
GLOBAL(stdc_count_ones_ull)
push r3
push r2
push r1
jsr stdc_count_ones_us
move r0, r1
pop r0
push r1
jsr stdc_count_ones_us
pop r1
add r0, r1, r1
pop r0
push r1
jsr stdc_count_ones_us
pop r1
add r0, r1, r1
pop r0
push r1
jsr stdc_count_ones_us
pop r1
add r0, r1, r0
rts

RSECT(stdc_count_zeros_uc)
EXTERN(stdc_count_ones_uc)
GLOBAL(stdc_count_zeros_uc)
not r0
scl r0, r0
jsr stdc_count_ones_uc
rts

RSECT(stdc_count_zeros_ui)
EXTERN(stdc_count_ones_us)
GLOBAL(stdc_count_zeros_us)
GLOBAL(stdc_count_zeros_ui)
not r0
jsr stdc_count_ones_us
rts

RSECT(stdc_count_zeros_ul)
EXTERN(stdc_count_ones_ul)
GLOBAL(stdc_count_zeros_ul)
not r0
not r1
jsr stdc_count_ones_ul
rts

RSECT(stdc_count_zeros_ull)
EXTERN(stdc_count_ones_ull)
GLOBAL(stdc_count_zeros_ull)
not r0
not r1
not r2
not r3
jsr stdc_count_ones_ull
rts

RSECT(stdc_has_single_bit_ui)
GLOBAL(stdc_has_single_bit_ui)
GLOBAL(stdc_has_single_bit_us)
GLOBAL(stdc_has_single_bit_uc)
cmp r0, 0
beq stdc_has_single_bit_uc_false
move r0, r1
dec r1
and r0, r1, r0
bz stdc_has_single_bit_uc_true
clr r0
stdc_has_single_bit_uc_false:
rts
stdc_has_single_bit_uc_true:
ldi r0, 1
rts


RSECT(stdc_has_single_bit_ul)
EXTERN(stdc_has_single_bit_us)
GLOBAL(stdc_has_single_bit_ul)
cmp r0, 0
bne stdc_has_single_bit_ul_test_lo
move r1, r0
bnz stdc_has_single_bit_ul_test_hi
rts

stdc_has_single_bit_ul_test_lo:
cmp r1, 0
bne stdc_has_single_bit_ul_end
stdc_has_single_bit_ul_test_hi:
jsr stdc_has_single_bit_us
rts

stdc_has_single_bit_ul_end:
clr r0
rts


RSECT(stdc_has_single_bit_ull)
EXTERN(stdc_has_single_bit_us)
GLOBAL(stdc_has_single_bit_ull)
cmp r0, 0
bne stdc_has_single_bit_ull_test_0
move r1, r0
bnz stdc_has_single_bit_ull_test_1
move r2, r0
bnz stdc_has_single_bit_ull_test_2
move r3, r0
bnz stdc_has_single_bit_ull_test_3
rts

stdc_has_single_bit_ull_test_0:
cmp r1, 0
bne stdc_has_single_bit_ull_end
stdc_has_single_bit_ull_test_1:
cmp r2, 0
bne stdc_has_single_bit_ull_end
stdc_has_single_bit_ull_test_2:
cmp r3, 0
bne stdc_has_single_bit_ull_end
stdc_has_single_bit_ull_test_3:
jsr stdc_has_single_bit_us
rts

stdc_has_single_bit_ull_end:
clr r0
rts


RSECT(stdc_bit_width_uc)
EXTERN(stdc_leading_zeros_uc)
GLOBAL(stdc_bit_width_uc)
jsr stdc_leading_zeros_uc
sub r0, 8
neg r0
rts

RSECT(stdc_bit_width_ui)
EXTERN(stdc_leading_zeros_us)
GLOBAL(stdc_bit_width_us)
GLOBAL(stdc_bit_width_ui)
jsr stdc_leading_zeros_us
sub r0, 16
neg r0
rts

RSECT(stdc_bit_width_ul)
EXTERN(stdc_leading_zeros_ul)
GLOBAL(stdc_bit_width_ul)
jsr stdc_leading_zeros_ul
sub r0, 32
neg r0
rts

RSECT(stdc_bit_width_ull)
EXTERN(stdc_leading_zeros_ull)
GLOBAL(stdc_bit_width_ull)
jsr stdc_leading_zeros_ull
sub r0, 64
neg r0
rts

RSECT(stdc_bit_floor_uc)
GLOBAL(stdc_bit_floor_uc)
move r0, r1
shr r1, r1, 1
or r1, r0, r0

move r0, r1
shr r1, r1, 2
or r1, r0, r0

move r0, r1
shr r1, r1, 4
or r1, r0, r0

move r0, r1
shr r1, r1, 1
sub r0, r1, r0
rts


RSECT(stdc_bit_floor_ui)
GLOBAL(stdc_bit_floor_us)
GLOBAL(stdc_bit_floor_ui)
move r0, r1
shr r1, r1, 1
or r1, r0, r0

move r0, r1
shr r1, r1, 2
or r1, r0, r0

move r0, r1
shr r1, r1, 4
or r1, r0, r0

move r0, r1
shr r1, r1, 8
or r1, r0, r0

move r0, r1
shr r1, r1, 1
sub r0, r1, r0
rts

RSECT(stdc_bit_floor_ul)
EXTERN(stdc_bit_floor_us)
GLOBAL(stdc_bit_floor_ul)
cmp r1, 0
bne stdc_bit_floor_ul_high
jsr stdc_bit_floor_us
clr r1
rts

stdc_bit_floor_ul_high:
move r1, r0
jsr stdc_bit_floor_us
move r0, r1
clr r0
rts

RSECT(stdc_bit_floor_ull)
EXTERN(stdc_bit_floor_us)
GLOBAL(stdc_bit_floor_ull)
cmp r3, 0
bne stdc_bit_floor_ull_3
cmp r2, 0
bne stdc_bit_floor_ull_2
cmp r1, 0
bne stdc_bit_floor_ull_1

jsr stdc_bit_floor_us
clr r1
clr r2
clr r3
rts

stdc_bit_floor_ull_1:
move r1, r0
jsr stdc_bit_floor_us
move r0, r1
clr r0
clr r2
clr r3
rts

stdc_bit_floor_ull_2:
move r2, r0
jsr stdc_bit_floor_us
move r0, r2
clr r0
clr r1
clr r3
rts

stdc_bit_floor_ull_3:
move r3, r0
jsr stdc_bit_floor_us
move r0, r3
clr r0
clr r1
clr r2
rts

RSECT(stdc_bit_ceil_uc)
EXTERN(stdc_bit_floor_uc)
GLOBAL(stdc_bit_ceil_uc)
cmp r0, 1
bhi stdc_bit_ceil_uc_calc
ldi r0, 1
rts
stdc_bit_ceil_uc_calc:
dec r0
jsr stdc_bit_floor_uc
shl r0, r0, 1
scl r0, r0
rts

RSECT(stdc_bit_ceil_ui)
EXTERN(stdc_bit_floor_us)
GLOBAL(stdc_bit_ceil_us)
GLOBAL(stdc_bit_ceil_ui)
cmp r0, 1
bhi stdc_bit_ceil_us_calc
ldi r0, 1
rts
stdc_bit_ceil_us_calc:
dec r0
jsr stdc_bit_floor_us
shl r0, r0, 1
rts

RSECT(stdc_bit_ceil_ul)
EXTERN(stdc_bit_floor_ul)
GLOBAL(stdc_bit_ceil_ul)
cmp r1, 0
bne stdc_bit_ceil_ul_calc
cmp r0, 1
bhi stdc_bit_ceil_ul_calc
ldi r0, 1
clr r1
rts

stdc_bit_ceil_ul_calc:
cmp r0, 0
bne stdc_bit_ceil_ul_nobor
dec r1
stdc_bit_ceil_ul_nobor:
dec r0

jsr stdc_bit_floor_ul
shl r0, r0, 1
rcl r1, r1, 1
rts

RSECT(stdc_bit_ceil_ull)
EXTERN(stdc_bit_floor_ull)
GLOBAL(stdc_bit_ceil_ull)
cmp r3, 0
bne stdc_bit_ceil_ull_calc
cmp r2, 0
bne stdc_bit_ceil_ull_calc
cmp r1, 0
bne stdc_bit_ceil_ull_calc
cmp r0, 1
bhi stdc_bit_ceil_ull_calc
ldi r0, 1
rts

stdc_bit_ceil_ull_calc:
cmp r0, 0
bne stdc_bit_ceil_ull_nobor1
cmp r1, 0
bne stdc_bit_ceil_ull_nobor2
cmp r2, 0
bne stdc_bit_ceil_ull_nobor3
dec r3
stdc_bit_ceil_ull_nobor3:
dec r2
stdc_bit_ceil_ull_nobor2:
dec r1
stdc_bit_ceil_ull_nobor1:
dec r0

jsr stdc_bit_floor_ull

shl r0, r0, 1
rcl r1, r1, 1
rcl r2, r2, 1
rcl r3, r3, 1
rts

END
