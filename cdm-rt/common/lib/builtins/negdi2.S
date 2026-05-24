#include "cdm_asm.inc"

RSECT(__negdi2)

GLOBAL(__negdi2)

    neg r0
    bcs __negdi2_c0
    not r1
    not r2
    not r3
    rts
__negdi2_c0:
    neg r1
    bcs __negdi2_c1
    not r2
    not r3
    rts
__negdi2_c1:
    neg r2
    bcs __negdi2_c2
    not r3
    rts
__negdi2_c2:
    neg r3
    rts

END
