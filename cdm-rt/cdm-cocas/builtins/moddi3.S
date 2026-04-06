#include "cdm_asm.inc"

RSECT(__moddi3)

EXTERN(__udivmoddi4)

GLOBAL(__moddi3)

    push fp
    ldsp fp
    addsp -34
    ssw r4, -2
    ssw r5, -4
    ssw r6, -6

    ldi r5, 0
    ssw r5, -34

    tst r3
    bpl __moddi3_a_pos
    ldi r5, 1
    ssw r5, -24
    neg r0
    bz __moddi3_na_c0
    not r1
    not r2
    not r3
    br __moddi3_a_pos
__moddi3_na_c0:
    neg r1
    bz __moddi3_na_c1
    not r2
    not r3
    br __moddi3_a_pos
__moddi3_na_c1:
    neg r2
    bz __moddi3_na_c2
    not r3
    br __moddi3_a_pos
__moddi3_na_c2:
    neg r3
__moddi3_a_pos:
    lsw r5, 18
    tst r5
    bpl __moddi3_b_pos
    lsw r5, 12
    neg r5
    ssw r5, 12
    bz __moddi3_nb_c0
    lsw r6, 14
    not r6
    ssw r6, 14
    lsw r6, 16
    not r6
    ssw r6, 16
    lsw r6, 18
    not r6
    ssw r6, 18
    br __moddi3_b_pos
__moddi3_nb_c0:
    lsw r5, 14
    neg r5
    ssw r5, 14
    bz __moddi3_nb_c1
    lsw r6, 16
    not r6
    ssw r6, 16
    lsw r6, 18
    not r6
    ssw r6, 18
    br __moddi3_b_pos
__moddi3_nb_c1:
    lsw r5, 16
    neg r5
    ssw r5, 16
    bz __moddi3_nb_c2
    lsw r6, 18
    not r6
    ssw r6, 18
    br __moddi3_b_pos
__moddi3_nb_c2:
    lsw r5, 18
    neg r5
    ssw r5, 18

__moddi3_b_pos:
    lsw r5, 12
    ssw r5, -26
    lsw r5, 14
    ssw r5, -24
    lsw r5, 16
    ssw r5, -22
    lsw r5, 18
    ssw r5, -20

    move fp, r5
    sub r5, 14
    ssw r5, -18

    jsr __udivmoddi4

    lsw r0, -14
    lsw r1, -12
    lsw r2, -10
    lsw r3, -8

    lsw r5, -34
    tst r5
    bz __moddi3_done
    neg r0
    bz __moddi3_nr_c0
    not r1
    not r2
    not r3
    br __moddi3_done
__moddi3_nr_c0:
    neg r1
    bz __moddi3_nr_c1
    not r2
    not r3
    br __moddi3_done
__moddi3_nr_c1:
    neg r2
    bz __moddi3_nr_c2
    not r3
    br __moddi3_done
__moddi3_nr_c2:
    neg r3

__moddi3_done:
    lsw r4, -2
    lsw r5, -4
    lsw r6, -6
    stsp fp
    pop fp
    rts

END
