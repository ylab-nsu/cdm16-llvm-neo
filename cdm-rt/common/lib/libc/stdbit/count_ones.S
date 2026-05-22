#include "cdm_asm.inc"

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

END
