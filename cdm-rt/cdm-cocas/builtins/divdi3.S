#include "cdm_asm.inc"

RSECT(__divdi3)

GLOBAL(__divdi3)

    push fp
    ldsp fp
    push r4
    push r5
    push r6
    addsp -18

    ldi r5, 0
    ssw r5, -24

    tst r3
    bpl __divdi3_a_pos
    ldi r5, 1
    ssw r5, -24
    neg r0
    bz __divdi3_na_c0
    ldi r5, -1
    xor r1, r5, r1
    xor r2, r5, r2
    xor r3, r5, r3
    br __divdi3_a_pos
__divdi3_na_c0:
    neg r1
    bz __divdi3_na_c1
    ldi r5, -1
    xor r2, r5, r2
    xor r3, r5, r3
    br __divdi3_a_pos
__divdi3_na_c1:
    neg r2
    bz __divdi3_na_c2
    ldi r5, -1
    xor r3, r5, r3
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
    ldi r5, -1
    lsw r6, 14
    xor r6, r5, r6
    ssw r6, 14
    lsw r6, 16
    xor r6, r5, r6
    ssw r6, 16
    lsw r6, 18
    xor r6, r5, r6
    ssw r6, 18
    br __divdi3_b_pos
__divdi3_nb_c0:
    lsw r5, 14
    neg r5
    ssw r5, 14
    bz __divdi3_nb_c1
    ldi r5, -1
    lsw r6, 16
    xor r6, r5, r6
    ssw r6, 16
    lsw r6, 18
    xor r6, r5, r6
    ssw r6, 18
    br __divdi3_b_pos
__divdi3_nb_c1:
    lsw r5, 16
    neg r5
    ssw r5, 16
    bz __divdi3_nb_c2
    ldi r5, -1
    lsw r6, 18
    xor r6, r5, r6
    ssw r6, 18
    br __divdi3_b_pos
__divdi3_nb_c2:
    lsw r5, 18
    neg r5
    ssw r5, 18
__divdi3_b_pos:

    ssw r0, -8
    ssw r1, -10
    ssw r2, -12
    ssw r3, -14

    ldi r5, 0
    ssw r5, -16
    ssw r5, -18
    ssw r5, -20
    ssw r5, -22

    ldi r4, 64

__divdi3_loop:
    tst r4
    bz __divdi3_div_done

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

    lsw r5, -16
    rcl r5
    ssw r5, -16
    lsw r5, -18
    rcl r5
    ssw r5, -18
    lsw r5, -20
    rcl r5
    ssw r5, -20
    lsw r5, -22
    rcl r5
    ssw r5, -22

    lsw r5, -22
    lsw r6, 18
    cmp r5, r6
    bhi __divdi3_sub
    blo __divdi3_skip
    lsw r5, -20
    lsw r6, 16
    cmp r5, r6
    bhi __divdi3_sub
    blo __divdi3_skip
    lsw r5, -18
    lsw r6, 14
    cmp r5, r6
    bhi __divdi3_sub
    blo __divdi3_skip
    lsw r5, -16
    lsw r6, 12
    cmp r5, r6
    blo __divdi3_skip

__divdi3_sub:
    lsw r5, -16
    lsw r6, 12
    sub r5, r6, r5
    ssw r5, -16
    lsw r5, -18
    lsw r6, 14
    subc r5, r6, r5
    ssw r5, -18
    lsw r5, -20
    lsw r6, 16
    subc r5, r6, r5
    ssw r5, -20
    lsw r5, -22
    lsw r6, 18
    subc r5, r6, r5
    ssw r5, -22

    lsw r5, -8
    inc r5
    ssw r5, -8

__divdi3_skip:
    dec r4
    br __divdi3_loop

__divdi3_div_done:
    lsw r0, -8
    lsw r1, -10
    lsw r2, -12
    lsw r3, -14

    lsw r5, -24
    tst r5
    bz __divdi3_done
    neg r0
    bz __divdi3_nr_c0
    ldi r5, -1
    xor r1, r5, r1
    xor r2, r5, r2
    xor r3, r5, r3
    br __divdi3_done
__divdi3_nr_c0:
    neg r1
    bz __divdi3_nr_c1
    ldi r5, -1
    xor r2, r5, r2
    xor r3, r5, r3
    br __divdi3_done
__divdi3_nr_c1:
    neg r2
    bz __divdi3_nr_c2
    ldi r5, -1
    xor r3, r5, r3
    br __divdi3_done
__divdi3_nr_c2:
    neg r3

__divdi3_done:
    addsp 18
    pop r6
    pop r5
    pop r4
    stsp fp
    pop fp
    rts

END
