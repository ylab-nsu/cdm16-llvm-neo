#include "cdm_asm.inc"

RSECT(__mulosi4)

EXTERN(__mulsi3)
EXTERN(__divsi3)

GLOBAL(__mulosi4)

    push fp
    ldsp fp
    addsp -16
    ssw r4, -2
    ssw r5, -4

    ssw r0, -6
    ssw r1, -8
    ssw r2, -10
    ssw r3, -12

    jsr __mulsi3

    ssw r0, -14
    ssw r1, -16

    lsw r4, -10
    lsw r5, -12
    or r4, r5, r4
    bz __mulosi4_no_overflow

    lsw r4, -10
    inc r4
    bnz __mulosi4_div_check
    lsw r4, -12
    inc r4
    bnz __mulosi4_div_check

    lsw r4, -6
    tst r4
    bnz __mulosi4_no_overflow
    lsw r4, -8
    tst r4
    bpl __mulosi4_no_overflow
    shl r4
    tst r4
    bnz __mulosi4_no_overflow
    br __mulosi4_set_overflow

__mulosi4_div_check:
    lsw r0, -14
    lsw r1, -16
    lsw r2, -10
    lsw r3, -12
    jsr __divsi3

    lsw r4, -6
    cmp r0, r4
    bnz __mulosi4_set_overflow
    lsw r4, -8
    cmp r1, r4
    bnz __mulosi4_set_overflow

__mulosi4_no_overflow:
    ldi r4, 0
    lsw r5, 12
    stw r5, r4
    br __mulosi4_done

__mulosi4_set_overflow:
    ldi r4, 1
    lsw r5, 12
    stw r5, r4

__mulosi4_done:
    lsw r0, -14
    lsw r1, -16
    lsw r4, -2
    lsw r5, -4
    stsp fp
    pop fp
    rts

END