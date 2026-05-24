#include "cdm_asm.inc"

RSECT(__adddi3)

GLOBAL(__adddi3)

    push fp
    ldsp fp
    addsp -2
    ssw r5, -2

    lsw r5, 12
    add r0, r5, r0
    lsw r5, 14
    addc r1, r5, r1
    lsw r5, 16
    addc r2, r5, r2
    lsw r5, 18
    addc r3, r5, r3

    lsw r5, -2
    stsp fp
    pop fp
    rts

END