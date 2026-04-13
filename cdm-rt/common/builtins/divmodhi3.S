#include "cdm_asm.inc"

RSECT(__udivmodhi3)

GLOBAL(__udivmodhi3)

    ldi r2, 0
    ldi r3, 16
__udivmodhi3_loop:
    tst r3
    bz __udivmodhi3_end
    shl r0
    rcl r2
    cmp r2, r1
    blo __udivmodhi3_skip
    sub r2, r1, r2
    inc r0
__udivmodhi3_skip:
    dec r3
    br __udivmodhi3_loop
__udivmodhi3_end:
    rts

RSECT(__udivhi3)

EXTERN(__udivmodhi3)

GLOBAL(__udivhi3)

    jsr __udivmodhi3
    rts

RSECT(__umodhi3)

EXTERN(__udivmodhi3)

GLOBAL(__umodhi3)

    jsr __udivmodhi3
    move r2, r0
    rts

RSECT(__divhi3)

EXTERN(__udivmodhi3)

GLOBAL(__divhi3)

    push r4
    ldi r4, 0

    tst r0
    bpl __divhi3_r0_pos
    inc r4
    neg r0
__divhi3_r0_pos:
    tst r1
    bpl __divhi3_r1_pos
    dec r4
    neg r1
__divhi3_r1_pos:
    jsr __udivmodhi3
    tst r4
    bz __divhi3_done
    neg r0
__divhi3_done:
    pop r4
    rts

RSECT(__modhi3)

EXTERN(__udivmodhi3)

GLOBAL(__modhi3)

    push r4
    ldi r4, 0

    tst r0
    bpl __modhi3_r0_pos
    inc r4
    neg r0
__modhi3_r0_pos:
    tst r1
    bpl __modhi3_r1_pos
    dec r4
    neg r1
__modhi3_r1_pos:
    jsr __udivmodhi3
    move r2, r0
    tst r4
    bz __modhi3_done
    neg r0
__modhi3_done:
    pop r4
    rts

END
