rsect __sync_fetch_and_xor_2

__sync_fetch_and_xor_2>
    push r4
    ldps r4
    di
    ldw r0, r2
    xor r2, r1, r1
    stw r0, r1
    move r2, r0
    stps r4
    pop r4
    rts

end.