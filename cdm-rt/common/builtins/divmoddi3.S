#include "cdm_asm.inc"

RSECT(__udivmoddi4)

GLOBAL(__udivmoddi4)

    push fp
    ldsp fp
    addsp -14
    ssw r4, -2
    ssw r5, -4
    ssw r6, -6

    ssw r0, -8
    ssw r1, -10
    ssw r2, -12
    ssw r3, -14

    ldi r0, 0
    ldi r1, 0
    ldi r2, 0
    ldi r3, 0

    ldi r4, 64

__udivmoddi4_loop:
    tst r4
    bz __udivmoddi4_div_done

    lsw r5, -8
    shl r5
    ssw r5, -8
    lsw r5, -10
    rcl r5
    ssw r5, -10
    lsw r5, -12
    rcl r5
    ssw r5, -12
    lsw r5, -14
    rcl r5
    ssw r5, -14

    rcl r0
    rcl r1
    rcl r2
    rcl r3

    lsw r5, 18
    cmp r3, r5
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, 16
    cmp r2, r5
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, 14
    cmp r1, r5
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, 12
    cmp r0, r5
    blo __udivmoddi4_skip

__udivmoddi4_sub:
    lsw r5, 12
    sub r0, r5, r0
    lsw r5, 14
    subc r1, r5, r1
    lsw r5, 16
    subc r2, r5, r2
    lsw r5, 18
    subc r3, r5, r3

    lsw r5, -8
    inc r5
    ssw r5, -8

__udivmoddi4_skip:
    dec r4
    br __udivmoddi4_loop

__udivmoddi4_div_done:
    lsw r5, 20
    tst r5
    bz __udivmoddi4_no_rem
    stw r5, r0
    add r5, 2
    stw r5, r1
    add r5, 2
    stw r5, r2
    add r5, 2
    stw r5, r3

__udivmoddi4_no_rem:
    lsw r0, -8
    lsw r1, -10
    lsw r2, -12
    lsw r3, -14

    lsw r4, -2
    lsw r5, -4
    lsw r6, -6
    stsp fp
    pop fp
    rts

RSECT(__udivdi3)

EXTERN(__udivmoddi4)

GLOBAL(__udivdi3)

    push fp
    ldsp fp
    addsp -20
    ssw r4, -2

    lsw r4, 12
    ssw r4, -12
    lsw r4, 14
    ssw r4, -10
    lsw r4, 16
    ssw r4, -8
    lsw r4, 18
    ssw r4, -6

    ldi r4, 0
    ssw r4, -4

    jsr __udivmoddi4

    lsw r4, -2
    stsp fp
    pop fp
    rts

RSECT(__umoddi3)

EXTERN(__udivmoddi4)

GLOBAL(__umoddi3)

    push fp
    ldsp fp
    addsp -30
    ssw r4, -2
    ssw r5, -4

    lsw r4, 12
    ssw r4, -22
    lsw r4, 14
    ssw r4, -20
    lsw r4, 16
    ssw r4, -18
    lsw r4, 18
    ssw r4, -16

    move fp, r4
    sub r4, 12
    ssw r4, -14

    jsr __udivmoddi4

    lsw r0, -12
    lsw r1, -10
    lsw r2, -8
    lsw r3, -6

    lsw r4, -2
    lsw r5, -4
    stsp fp
    pop fp
    rts

RSECT(__divdi3)

EXTERN(__udivmoddi4)

GLOBAL(__divdi3)

    push fp
    ldsp fp
    addsp -24
    ssw r4, -2
    ssw r5, -4
    ssw r6, -6

    ldi r5, 0
    ssw r5, -24

    tst r3
    bpl __divdi3_a_pos
    ldi r5, 1
    ssw r5, -24
    neg r0
    bz __divdi3_na_c0
    not r1
    not r2
    not r3
    br __divdi3_a_pos
__divdi3_na_c0:
    neg r1
    bz __divdi3_na_c1
    not r2
    not r3
    br __divdi3_a_pos
__divdi3_na_c1:
    neg r2
    bz __divdi3_na_c2
    not r3
    br __divdi3_a_pos
__divdi3_na_c2:
    neg r3
__divdi3_a_pos:
    lsw r5, 18
    tst r5
    bpl __divdi3_b_pos
    lsw r5, -24
    ldi r6, 1
    xor r5, r6, r5
    ssw r5, -24
    lsw r5, 12
    neg r5
    ssw r5, 12
    bz __divdi3_nb_c0
    lsw r6, 14
    not r6
    ssw r6, 14
    lsw r6, 16
    not r6
    ssw r6, 16
    lsw r6, 18
    not r6
    ssw r6, 18
    br __divdi3_b_pos
__divdi3_nb_c0:
    lsw r5, 14
    neg r5
    ssw r5, 14
    bz __divdi3_nb_c1
    lsw r6, 16
    not r6
    ssw r6, 16
    lsw r6, 18
    not r6
    ssw r6, 18
    br __divdi3_b_pos
__divdi3_nb_c1:
    lsw r5, 16
    neg r5
    ssw r5, 16
    bz __divdi3_nb_c2
    lsw r6, 18
    not r6
    ssw r6, 18
    br __divdi3_b_pos
__divdi3_nb_c2:
    lsw r5, 18
    neg r5
    ssw r5, 18

__divdi3_b_pos:
    lsw r5, 12
    ssw r5, -16
    lsw r5, 14
    ssw r5, -14
    lsw r5, 16
    ssw r5, -12
    lsw r5, 18
    ssw r5, -10

    ldi r5, 0
    ssw r5, -8

    jsr __udivmoddi4

    lsw r5, -24
    tst r5
    bz __divdi3_done
    neg r0
    bz __divdi3_nr_c0
    not r1
    not r2
    not r3
    br __divdi3_done
__divdi3_nr_c0:
    neg r1
    bz __divdi3_nr_c1
    not r2
    not r3
    br __divdi3_done
__divdi3_nr_c1:
    neg r2
    bz __divdi3_nr_c2
    not r3
    br __divdi3_done
__divdi3_nr_c2:
    neg r3

__divdi3_done:
    lsw r4, -2
    lsw r5, -4
    lsw r6, -6
    stsp fp
    pop fp
    rts

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
