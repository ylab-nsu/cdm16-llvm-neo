#include "cdm_asm.inc"

RSECT(__umoddi3)

EXTERN(__udivmoddi4)

GLOBAL(__umoddi3)

    push fp
    ldsp fp
    addsp -30
    ssw r4, -2
    ssw r5, -4

    lsw r4, 12
    ssw r4, -22
    lsw r4, 14
    ssw r4, -20
    lsw r4, 16
    ssw r4, -18
    lsw r4, 18
    ssw r4, -16

    move fp, r4
    sub r4, 12
    ssw r4, -14

    jsr __udivmoddi4

    lsw r0, -12
    lsw r1, -10
    lsw r2, -8
    lsw r3, -6

    lsw r4, -2
    lsw r5, -4
    stsp fp
    pop fp
    rts

END
