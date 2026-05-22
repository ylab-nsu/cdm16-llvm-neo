#include "cdm_asm.inc"

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
