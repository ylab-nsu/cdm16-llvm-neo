rsect __udivmoddi4

__udivmoddi4>

    push fp
    ldsp fp
    addsp -22
    ssw r4, -2
    ssw r5, -4
    ssw r6, -6

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
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, -20
    lsw r6, 16
    cmp r5, r6
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, -18
    lsw r6, 14
    cmp r5, r6
    bhi __udivmoddi4_sub
    blo __udivmoddi4_skip
    lsw r5, -16
    lsw r6, 12
    cmp r5, r6
    blo __udivmoddi4_skip

__udivmoddi4_sub:
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

__udivmoddi4_skip:
    dec r4
    br __udivmoddi4_loop

__udivmoddi4_div_done:
    lsw r5, 20
    tst r5
    bz __udivmoddi4_no_rem
    lsw r6, -16
    stw r5, r6
    add r5, 2
    lsw r6, -18
    stw r5, r6
    add r5, 2
    lsw r6, -20
    stw r5, r6
    add r5, 2
    lsw r6, -22
    stw r5, r6

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

end.
