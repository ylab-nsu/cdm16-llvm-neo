#include "cdm_asm.inc"

RSECT(__subdi3)

GLOBAL(__subdi3)

    push fp
    ldsp fp
    addsp -2
    ssw r5, -2

    lsw r5, 12
    sub r0, r5, r0
    lsw r5, 14
    subc r1, r5, r1
    lsw r5, 16
    subc r2, r5, r2
    lsw r5, 18
    subc r3, r5, r3

    lsw r5, -2
    stsp fp
    pop fp
    rts

END