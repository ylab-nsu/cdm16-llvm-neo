rsect __divsi3

__udivsi3: ext

__divsi3>
    push r4
    ldi r4, 0

    tst r1
    bpl __divsi3_a_pos
    inc r4
    neg r1
    neg r0
    bz __divsi3_a_pos
    dec r1
__divsi3_a_pos:
    tst r3
    bpl __divsi3_b_pos
    dec r4
    neg r3
    neg r2
    bz __divsi3_b_pos
    dec r3
__divsi3_b_pos:
    jsr __udivsi3
    tst r4
    bz __divsi3_done
    neg r1
    neg r0
    bz __divsi3_done
    dec r1
__divsi3_done:
    pop r4
    rts

end.
