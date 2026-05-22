#include "cdm_asm.inc"

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

END
