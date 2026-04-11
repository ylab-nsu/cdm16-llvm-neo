#include "cdm_asm.inc"

RSECT(__umodhi3)

GLOBAL(__umodhi3)

    ldi r2, 0
    ldi r3, 16
__umodhi3_loop:
    tst r3
    bz __umodhi3_end
    shl r0
    rcl r2
    cmp r2, r1
    blo __umodhi3_skip
    sub r2, r1, r2
    inc r0
__umodhi3_skip:
    dec r3
    br __umodhi3_loop
__umodhi3_end:
    move r2, r0
    rts

END
