#include "cdm_asm.inc"

RSECT(.text.__divhi3)

EXTERN(__udivhi3)

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
    jsr __udivhi3
    tst r4
    bz __divhi3_done
    neg r0
__divhi3_done:
    pop r4
    rts

END
