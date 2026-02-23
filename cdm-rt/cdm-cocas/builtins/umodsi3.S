#include "cdm_asm.inc"

RSECT(__umodsi3)

GLOBAL(__umodsi3)

    push r4
    push r5
    push r6
    ldi r4, 0
    ldi r5, 0
    ldi r6, 32
__umodsi3_loop:
    tst r6
    bz __umodsi3_done
    shl r0
    rcl r1
    rcl r4
    rcl r5
    cmp r5, r3
    bhi __umodsi3_sub
    blo __umodsi3_skip
    cmp r4, r2
    blo __umodsi3_skip
__umodsi3_sub:
    sub r4, r2, r4
    subc r5, r3, r5
    inc r0
__umodsi3_skip:
    dec r6
    br __umodsi3_loop
__umodsi3_done:
    move r4, r0
    move r5, r1
    pop r6
    pop r5
    pop r4
    rts

END
