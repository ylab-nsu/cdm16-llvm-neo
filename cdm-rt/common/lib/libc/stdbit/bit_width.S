#include "cdm_asm.inc"

RSECT(stdc_bit_width_uc)
EXTERN(stdc_leading_zeros_uc)
GLOBAL(stdc_bit_width_uc)
jsr stdc_leading_zeros_uc
sub r0, 8
neg r0
rts

RSECT(stdc_bit_width_ui)
EXTERN(stdc_leading_zeros_us)
GLOBAL(stdc_bit_width_us)
GLOBAL(stdc_bit_width_ui)
jsr stdc_leading_zeros_us
sub r0, 16
neg r0
rts

RSECT(stdc_bit_width_ul)
EXTERN(stdc_leading_zeros_ul)
GLOBAL(stdc_bit_width_ul)
jsr stdc_leading_zeros_ul
sub r0, 32
neg r0
rts

RSECT(stdc_bit_width_ull)
EXTERN(stdc_leading_zeros_ull)
GLOBAL(stdc_bit_width_ull)
jsr stdc_leading_zeros_ull
sub r0, 64
neg r0
rts

END
