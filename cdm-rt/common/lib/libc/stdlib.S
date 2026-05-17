#include "cdm_asm.inc"

RSECT(stdlib)

GLOBAL(memalignment)
move r0, r1
neg r0
and r0, r1, r0
rts

END
