rsect __atomic_load

__atomic_load>
    push r4
    push r5
    ldps r4
    di
    ldw r1, r5
    stw r2, r5
    stps r4
    pop r5
    pop r4
    rts

end.