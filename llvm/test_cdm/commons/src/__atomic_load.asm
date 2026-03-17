rsect __atomic_load

__atomic_load>
    push r4
    push r5
    ldps r4
    di
__atomic_load_loop:
    tst r0
    bz __atomic_load_done
    ldw r1, r5
    stw r2, r5
    add r1, 2
    add r2, 2
    sub r0, 2
    br __atomic_load_loop
__atomic_load_done:
    stps r4
    pop r5
    pop r4
    rts

end.