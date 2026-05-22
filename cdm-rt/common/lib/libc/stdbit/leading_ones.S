#include "cdm_asm.inc"

RSECT(stdc_leading_ones_uc)
EXTERN(stdc_leading_zeros_uc)
GLOBAL(stdc_leading_ones_uc)
not r0
scl r0, r0
jsr stdc_leading_zeros_uc
rts

RSECT(stdc_leading_ones_ui)
EXTERN(stdc_leading_zeros_us)
GLOBAL(stdc_leading_ones_us)
GLOBAL(stdc_leading_ones_ui)
not r0
jsr stdc_leading_zeros_us
rts

RSECT(stdc_leading_ones_ul)
EXTERN(stdc_leading_zeros_ul)
GLOBAL(stdc_leading_ones_ul)
not r0
not r1
jsr stdc_leading_zeros_ul
rts

RSECT(stdc_leading_ones_ull)
EXTERN(stdc_leading_zeros_ull)
GLOBAL(stdc_leading_ones_ull)
not r0
not r1
not r2
not r3
jsr stdc_leading_zeros_ull
rts

END
