rsect __atomic_store

__atomic_store>
    push r4
    push r5
    ldps r4
    di
    ldw r2, r5
    stw r1, r5
    stps r4
    pop r5
    pop r4
    rts

end.