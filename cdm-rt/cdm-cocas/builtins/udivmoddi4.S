#include "cdm_asm.inc"

RSECT(__udivmoddi4)

GLOBAL(__udivmoddi4)

    push fp
    ldsp fp
    addsp -14
    ssw r4, -2
    ssw r5, -4
    ssw r6, -6

    ssw r0, -8
    ssw r1, -10
    ssw r2, -12
    ssw r3, -14

    ldi r0, 0
    ldi r1, 0
    ldi r2, 0
    ldi r3, 0

    ldi r4, 64

__udivmoddi4_loop:
    tst r4
    bz __udivmoddi4_div_done

    lsw r5, -8
    shl r5
    ssw r5, -8
    lsw r5, -10
    rcl r5
    ssw r5, -10
    lsw r5, -12
    rcl r5
    ssw r5, -12
    lsw r5, -14
    rcl r5
    ssw r5, -14

    rcl r0
    rcl r1
    rcl r2
    rcl r3

    lsw r5, 18
    cmp r3, r5
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, 16
    cmp r2, r5
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, 14
    cmp r1, r5
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, 12
    cmp r0, r5
    blo __udivmoddi4_skip

__udivmoddi4_sub:
    lsw r5, 12
    sub r0, r5, r0
    lsw r5, 14
    subc r1, r5, r1
    lsw r5, 16
    subc r2, r5, r2
    lsw r5, 18
    subc r3, r5, r3

    lsw r5, -8
    inc r5
    ssw r5, -8

__udivmoddi4_skip:
    dec r4
    br __udivmoddi4_loop

__udivmoddi4_div_done:
    lsw r5, 20
    tst r5
    bz __udivmoddi4_no_rem
    stw r5, r0
    add r5, 2
    stw r5, r1
    add r5, 2
    stw r5, r2
    add r5, 2
    stw r5, r3

__udivmoddi4_no_rem:
    lsw r0, -8
    lsw r1, -10
    lsw r2, -12
    lsw r3, -14

    lsw r4, -2
    lsw r5, -4
    lsw r6, -6
    stsp fp
    pop fp
    rts

END
