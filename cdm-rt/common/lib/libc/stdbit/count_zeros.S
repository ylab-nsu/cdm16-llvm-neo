#include "cdm_asm.inc"

RSECT(stdc_count_zeros_uc)
EXTERN(stdc_count_ones_uc)
GLOBAL(stdc_count_zeros_uc)
not r0
scl r0, r0
jsr stdc_count_ones_uc
rts

RSECT(stdc_count_zeros_ui)
EXTERN(stdc_count_ones_us)
GLOBAL(stdc_count_zeros_us)
GLOBAL(stdc_count_zeros_ui)
not r0
jsr stdc_count_ones_us
rts

RSECT(stdc_count_zeros_ul)
EXTERN(stdc_count_ones_ul)
GLOBAL(stdc_count_zeros_ul)
not r0
not r1
jsr stdc_count_ones_ul
rts

RSECT(stdc_count_zeros_ull)
EXTERN(stdc_count_ones_ull)
GLOBAL(stdc_count_zeros_ull)
not r0
not r1
not r2
not r3
jsr stdc_count_ones_ull
rts

END
