#include "cdm_asm.inc"

RSECT(__mulsi3)

GLOBAL(__mulsi3)

    push r4
    push r5
    push r6
    ldi r4, 0
    ldi r5, 0
    or r2, r3, r6
    bz __mulsi3_end
__mulsi3_loop_body:
    shr r3
    rcr r2
    bcc __mulsi3_skip_add
    add r0, r4, r4
    addc r1, r5, r5
__mulsi3_skip_add:
    shl r0
    rcl r1
    or r2, r3, r6
    bnz __mulsi3_loop_body
__mulsi3_end:
    move r4, r0
    move r5, r1
    pop r6
    pop r5
    pop r4
    rts


END