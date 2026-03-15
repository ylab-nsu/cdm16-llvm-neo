rsect __sync_fetch_and_max_2

__sync_fetch_and_max_2>
    push r4
    ldps r4
    di
    ldw r0, r2
    cmp r2, r1
    bge __sync_fetch_and_max_2_done
    stw r0, r1
__sync_fetch_and_max_2_done:
    move r2, r0
    stps r4
    pop r4
    rts

end.