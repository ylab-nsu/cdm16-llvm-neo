rsect __mulosi4

__mulosi4>
    push fp
    ldsp fp
    push r4
    push r5
    push r6
    addsp -10

    ldi r6, 0
    ssw r6, -8

    tst r1
    bpl __mulosi4_a_pos
    ldi r6, 1
    ssw r6, -8
    neg r1
    neg r0

    bz __mulosi4_a_pos
    dec r1

__mulosi4_a_pos:
    tst r3
    bpl __mulosi4_b_pos
    lsw r6, -8
    ldi r4, 1
    xor r6, r4, r6
    ssw r6, -8
    neg r3
    neg r2

    bz __mulosi4_b_pos
    dec r3

__mulosi4_b_pos:
    ldi r6, 0
    ssw r6, -10
    ssw r6, -12

    ldi r4, 0
    ldi r5, 0
    ssw r6, -14
    ssw r6, -16

    or r2, r3, r6
    bz __mulosi4_mul_done

__mulosi4_loop:
    shr r3
    rcr r2
    bcc __mulosi4_skip_add

    add r0, r4, r4
    addc r1, r5, r5

    push r4
    lsw r4, -10
    lsw r6, -14
    addc r4, r6
    ssw r6, -14
    lsw r4, -12
    lsw r6, -16
    addc r4, r6
    ssw r6, -16
    pop r4

__mulosi4_skip_add:
    shl r0
    rcl r1
    lsw r6, -10
    rcl r6
    ssw r6, -10
    lsw r6, -12
    rcl r6
    ssw r6, -12

    or r2, r3, r6
    bnz __mulosi4_loop

__mulosi4_mul_done:
    lsw r6, -8
    tst r6
    bz __mulosi4_check_overflow

    ldi r6, -1
    xor r4, r6, r4
    xor r5, r6, r5
    lsw r0, -14
    xor r0, r6, r0
    ssw r0, -14
    lsw r0, -16
    xor r0, r6, r0
    ssw r0, -16

    ldi r6, 0
    inc r4
    addc r6, r5
    lsw r0, -14
    addc r6, r0
    ssw r0, -14
    lsw r0, -16
    addc r6, r0
    ssw r0, -16

__mulosi4_check_overflow:
    tst r5
    bmi __mulosi4_neg_result

    lsw r6, -14
    tst r6
    bnz __mulosi4_set_overflow

    lsw r6, -16
    tst r6
    bnz __mulosi4_set_overflow
    br __mulosi4_no_overflow

__mulosi4_neg_result:
    lsw r6, -14
    inc r6
    bnz __mulosi4_set_overflow
    lsw r6, -16
    inc r6
    bnz __mulosi4_set_overflow
    br __mulosi4_no_overflow
    
__mulosi4_set_overflow:
    ldi r6, 1
    lsw r0, 12
    stw r0, r6
    br __mulosi4_done

__mulosi4_no_overflow:
    ldi r6, 0
    lsw r0, 12
    stw r0, r6
    br __mulosi4_done

__mulosi4_done:
    move r4, r0
    move r5, r1
    addsp 10
    pop r6
    pop r5
    pop r4
    stsp fp
    pop fp
    rts

end.
