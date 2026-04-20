#include "cdm_asm.inc"

RSECT(__udivmodsi3)

GLOBAL(__udivmodsi3)

    push r4
    push r5
    push r6

    or r2, r3, r4
    bnz __udivmodsi3_nonzero
    ZERO
__udivmodsi3_nonzero:
    ldi r4, 0
    ldi r5, 0
    ldi r6, 32

__udivmodsi3_loop:
    tst r6
    bz __udivmodsi3_done
    shl r0
    rcl r1
    rcl r4
    rcl r5
    cmp r5, r3
    bhi __udivmodsi3_sub
    blo __udivmodsi3_skip
    cmp r4, r2
    blo __udivmodsi3_skip
__udivmodsi3_sub:
    sub r4, r2, r4
    subc r5, r3, r5
    inc r0
__udivmodsi3_skip:
    dec r6
    br __udivmodsi3_loop
__udivmodsi3_done:
    move r4, r2
    move r5, r3
    pop r6
    pop r5
    pop r4
    rts

RSECT(__udivsi3)

EXTERN(__udivmodsi3)

GLOBAL(__udivsi3)

    jsr __udivmodsi3
    rts

RSECT(__umodsi3)

EXTERN(__udivmodsi3)

GLOBAL(__umodsi3)

    jsr __udivmodsi3
    move r2, r0
    move r3, r1
    rts

RSECT(__divsi3)

EXTERN(__udivmodsi3)

GLOBAL(__divsi3)

    push r4
    ldi r4, 0

    tst r1
    bpl __divsi3_a_pos
    inc r4
    not r1
    neg r0
    bcc __divsi3_a_pos
    inc r1
__divsi3_a_pos:
    tst r3
    bpl __divsi3_b_pos
    dec r4
    not r3
    neg r2
    bcc __divsi3_b_pos
    inc r3
__divsi3_b_pos:
    jsr __udivmodsi3
    tst r4
    bz __divsi3_done
    not r1
    neg r0
    bcc __divsi3_done
    inc r1
__divsi3_done:
    pop r4
    rts

RSECT(__modsi3)

EXTERN(__udivmodsi3)

GLOBAL(__modsi3)

    push r4
    ldi r4, 0
    tst r1
    bpl __modsi3_a_pos
    inc r4
    not r1
    neg r0
    bcc __modsi3_a_pos
    inc r1
__modsi3_a_pos:
    tst r3
    bpl __modsi3_b_pos
    not r3
    neg r2
    bcc __modsi3_b_pos
    inc r3
__modsi3_b_pos:
    jsr __udivmodsi3
    move r2, r0
    move r3, r1
    tst r4
    bz __modsi3_done
    not r1
    neg r0
    bcc __modsi3_done
    inc r1
__modsi3_done:
    pop r4
    rts

END
