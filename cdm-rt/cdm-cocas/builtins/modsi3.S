#include "cdm_asm.inc"

RSECT(__modsi3)

EXTERN(__umodsi3)

GLOBAL(__modsi3)

    push r4
    ldi r4, 0
    tst r1
    bpl __modsi3_a_pos
    inc r4
    neg r1
    neg r0
    bz __modsi3_a_pos
    dec r1
__modsi3_a_pos:
    tst r3
    bpl __modsi3_b_pos
    neg r3
    neg r2
    bz __modsi3_b_pos
    dec r3
__modsi3_b_pos:
    jsr __umodsi3
    tst r4
    bz __modsi3_done
    neg r1
    neg r0
    bz __modsi3_done
    dec r1
__modsi3_done:
    pop r4
    rts

END
