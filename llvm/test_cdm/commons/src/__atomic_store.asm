rsect __atomic_store

__atomic_store>
    push r4
    push r5
    ldps r4
    di
__atomic_store_loop:
    tst r0
    bz __atomic_store_done
    ldw r2, r5
    stw r1, r5
    add r1, 2
    add r2, 2
    sub r0, 2
    br __atomic_store_loop
__atomic_store_done:
    stps r4
    pop r5
    pop r4
    rts

end.