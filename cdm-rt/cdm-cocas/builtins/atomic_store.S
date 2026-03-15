#include "cdm_asm.inc"

RSECT(__atomic_store)

GLOBAL(__atomic_store)
    push r4
    push r5
    ldps r4
    di
    ldw r1, r5
    stw r2, r5
    stps r4
    pop r5
    pop r4
    rts

END
