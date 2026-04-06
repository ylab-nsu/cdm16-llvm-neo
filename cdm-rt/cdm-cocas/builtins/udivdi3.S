#include "cdm_asm.inc"

RSECT(__udivdi3)

EXTERN(__udivmoddi4)

GLOBAL(__udivdi3)

    push fp
    ldsp fp
    addsp -20
    ssw r4, -2

    lsw r4, 12
    ssw r4, -12
    lsw r4, 14
    ssw r4, -10
    lsw r4, 16
    ssw r4, -8
    lsw r4, 18
    ssw r4, -6

    ldi r4, 0
    ssw r4, -4

    jsr __udivmoddi4

    lsw r4, -2
    stsp fp
    pop fp
    rts

END
