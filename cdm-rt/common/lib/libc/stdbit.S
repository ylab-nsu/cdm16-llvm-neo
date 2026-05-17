#include "cdm_asm.inc"

RSECT(stdbit)

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

END
