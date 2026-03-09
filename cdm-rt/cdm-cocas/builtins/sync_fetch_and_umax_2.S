#include "cdm_asm.inc"

RSECT(__sync_fetch_and_umax_2)

GLOBAL(__sync_fetch_and_umax_2)

    push r4
    ldps r4
    di
    ldw r0, r2
    cmp r2, r1
    bhs __sync_fetch_and_umax_2_done
    stw r0, r1
__sync_fetch_and_umax_2_done:
    move r2, r0
    stps r4
    pop r4
    rts

END