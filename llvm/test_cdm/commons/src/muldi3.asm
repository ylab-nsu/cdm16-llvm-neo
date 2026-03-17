rsect __muldi3

__muldi3>
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

  lsw r5, 12
  ssw r5, -16
  lsw r5, 14
  ssw r5, -18
  lsw r5, 16
  ssw r5, -20
  lsw r5, 18
  ssw r5, -22

  ldi r0, 0
  ldi r1, 0
  ldi r2, 0
  ldi r3, 0

  ldi r4, 64

__muldi3_loop:
  tst r4
  bz __muldi3_done

  lsw r5, -14
  shr r5
  ssw r5, -14
  lsw r5, -12
  rcr r5
  ssw r5, -12
  lsw r5, -10
  rcr r5
  ssw r5, -10
  lsw r5, -8
  rcr r5
  ssw r5, -8

  bcc __muldi3_no_add

  lsw r5, -16
  add r0, r5, r0
  lsw r5, -18
  addc r1, r5, r1
  lsw r5, -20
  addc r2, r5, r2
  lsw r5, -22
  addc r3, r5, r3

__muldi3_no_add:
  lsw r5, -16
  shl r5
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

  dec r4
  br __muldi3_loop

__muldi3_done:
  lsw r4, -2
  lsw r5, -4
  lsw r6, -6
  stsp fp
  pop fp
  rts

end.