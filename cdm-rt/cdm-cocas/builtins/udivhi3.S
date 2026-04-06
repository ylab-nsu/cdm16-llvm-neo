#include "cdm_asm.inc"

RSECT(__udivhi3)

GLOBAL(__udivhi3)

    ldi r2, 0
    ldi r3, 16
__udivhi3_loop:
    tst r3
    bz __udivhi3_end
    shl r0
    rcl r2
    cmp r2, r1
    blo __udivhi3_skip
    sub r2, r1, r2
    inc r0
__udivhi3_skip:
    dec r3
    br __udivhi3_loop
__udivhi3_end:
    rts

END
