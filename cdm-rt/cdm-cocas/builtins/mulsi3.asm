rsect __mulsi3

__mulhi3:ext

__mulsi3>
  # a = r1:r0, b = r3:r2
  # s = r1:r0
  push r5
  push r4
  
  push r3 # b_high
  push r0 # a_low
  push r2 # b_low
  push r1 # a_high

  move r2, r1 
  jsr __umullhi3 # r2 * r0 = r1:r0
  move r0, r4 # save p_low to r4 
  move r1, r5 # save p_high to r5
  
  pop r0 # a_high
  pop r1 # b_low
  jsr __mulhi3 # r0 = a_high * b_low
  add r5, r0, r5

  pop r0 # a_low
  pop r1 # b_high
  jsr __mulhi3 # r0 = a_low * b_high
  add r5, r0, r5

  move r4, r0 # p_low
  move r5, r1 # p_high

  pop r4
  pop r5

  # r1:r0 = s_high:s_low

  rts

__umullhi3:
  # a = r0, b = r1
  # s = r1:r0
  push r2
  push r3
  push r4

  ldi r2, 0
  ldi r3, 0
  ldi r4, 0

  while
    tst r1
  stays nz
    if 
      shr r1
    is cs
      add r2, r0, r2
      addc r3, r4, r3
    fi
    shl r0
    rcl r4
  wend

  move r2, r0
  move r3, r1

  pop r4
  pop r3
  pop r2

  rts

end.