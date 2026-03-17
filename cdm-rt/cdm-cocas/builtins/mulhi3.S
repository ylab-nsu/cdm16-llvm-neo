#include "cdm_asm.inc"

RSECT(.text.__mulhi3)

GLOBAL(__mulhi3)

    ldi r2, 0
    tst r1
    bz __mulhi3_end
__mulhi3_loop_body:
    shr r1
    bcc __mulhi3_skip_add
    add r0, r2
__mulhi3_skip_add:
    shl r0
    tst r1
    bnz __mulhi3_loop_body
__mulhi3_end:
    move r2, r0
    rts

END