#include "cdm_asm.inc"

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

END
