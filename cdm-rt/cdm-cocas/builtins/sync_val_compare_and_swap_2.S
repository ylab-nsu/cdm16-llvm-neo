#include "cdm_asm.inc"

RSECT(.text.__sync_val_compare_and_swap_2)

GLOBAL(__sync_val_compare_and_swap_2)

    push r4
    ldps r4
    di
    ldw r0, r3
    cmp r3, r1
    bnz __sync_val_compare_and_swap_2_done
    stw r0, r2
__sync_val_compare_and_swap_2_done:
    move r3, r0
    stps r4
    pop r4
    rts

END