rsect __umoddi3

__umoddi3>
  push fp
  ldsp fp
  push r4
  push r5
  push r6
  addsp -16

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

__umoddi3_loop:
  tst r4
  bz __umoddi3_done

  # Left shift A
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

  # Left shift R
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

  # Checking if R >= B
  lsw r5, -22
  lsw r6, 18
  cmp r5, r6
  bhi __umoddi3_sub
  blo __umoddi3_skip
  lsw r5, -20
  lsw r6, 16
  cmp r5, r6
  bhi __umoddi3_sub
  blo __umoddi3_skip
  lsw r5, -18
  lsw r6, 14
  cmp r5, r6
  bhi __umoddi3_sub
  blo __umoddi3_skip
  lsw r5, -16
  lsw r6, 12
  cmp r5, r6
  blo __umoddi3_skip

__umoddi3_sub:
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

__umoddi3_skip:
  dec r4
  br __umoddi3_loop

__umoddi3_done:
  lsw r0, -16
  lsw r1, -18
  lsw r2, -20
  lsw r3, -22

  addsp 16
  pop r6
  pop r5
  pop r4
  stsp fp
  pop fp
  rts

end.
