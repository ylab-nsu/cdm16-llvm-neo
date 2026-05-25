#include "cdm_asm.inc"

RSECT(test_utils)

GLOBAL(abort)
ldi r0, -1
halt

GLOBAL(exit)
halt

GLOBAL(printf)
rts

END
