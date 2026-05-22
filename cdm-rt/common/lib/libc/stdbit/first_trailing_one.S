#include "cdm_asm.inc"

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

END
