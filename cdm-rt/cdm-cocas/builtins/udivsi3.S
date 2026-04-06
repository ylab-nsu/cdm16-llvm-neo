#include "cdm_asm.inc"

RSECT(__udivsi3)

GLOBAL(__udivsi3)

    push r4
    push r5
    push r6

    ldi r4, 0
    ldi r5, 0
    ldi r6, 32

__udivsi3_loop:
    tst r6
    bz __udivsi3_done
    shl r0
    rcl r1
    rcl r4
    rcl r5
    cmp r5, r3
    bhi __udivsi3_sub
    blo __udivsi3_skip
    cmp r4, r2
    blo __udivsi3_skip
__udivsi3_sub:
    sub r4, r2, r4
    subc r5, r3, r5
    inc r0
__udivsi3_skip:
    dec r6
    br __udivsi3_loop
__udivsi3_done:
    pop r6
    pop r5
    pop r4
    rts

END
