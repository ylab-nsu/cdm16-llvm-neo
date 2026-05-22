#include "cdm_asm.inc"

RSECT(stdc_first_leading_zero_uc)
EXTERN(stdc_first_leading_one_uc)
GLOBAL(stdc_first_leading_zero_uc)
not r0
scl r0, r0
jsr stdc_first_leading_one_uc
rts

RSECT(stdc_first_leading_zero_ui)
EXTERN(stdc_first_leading_one_us)
GLOBAL(stdc_first_leading_zero_us)
GLOBAL(stdc_first_leading_zero_ui)
not r0
jsr stdc_first_leading_one_us
rts

RSECT(stdc_first_leading_zero_ul)
EXTERN(stdc_first_leading_one_ul)
GLOBAL(stdc_first_leading_zero_ul)
not r0
not r1
jsr stdc_first_leading_one_ul
rts

RSECT(stdc_first_leading_zero_ull)
EXTERN(stdc_first_leading_one_ull)
GLOBAL(stdc_first_leading_zero_ull)
not r0
not r1
not r2
not r3
jsr stdc_first_leading_one_ull
rts

END
