rsect __muldi3

__muldi3>
  push r7
  push r6
  push r5
  push r4

  push r3
  push r2
  push r1
  push r0

  clr r0
  clr r1
  clr r2
  clr r3


  # lsw     r4, -16 #в r4 - старшая часть второго аргумента числа b
  # move r4, r0
  # lsw     r5, -18 #в r5 - младшая часть второго аргумента числа b
  # move r5, r1
  # lsw     r6, -20 #в r6 - старшая часть первого аргумента числа b
  # move r6, r2
  # lsw     r7, -22 #в r7 - младшая часть первого аргумента числа b
  # move r7, r3
  
  # int S = 0;
  #   while (A != 0) {
  #    if (A & 1)
  #      S += B;
  #    A = ((unsigned int) A) >> 1;
  #    B <<= 1;
  #  }
  #  return S;

  ldi r4, 64
  ldi r5, 0
  ldi r6, 0

  while
    tst r4
  stays nz
    if
      addsp 6
      pop r5
      shr r5
      push r5

      addsp -2
      pop r5
      rcr r5
      push r5

      addsp -2
      pop r5
      rcr r5
      push r5

      addsp -2
      pop r5
      rcr r5
      push r5
    is cs
      lsw r5, -22
      add r0, r5, r0

      lsw r5, -20
      addc r1, r5, r1

      lsw r5, -18
      addc r2, r5, r2

      lsw r5, -16
      addc r3, r5, r3
    fi
    lsw r5, -22
    shl r5
    ssw r5, -22

    lsw r5, -20
    rcl r5
    ssw r5, -20

    lsw r5, -18
    rcl r5
    ssw r5, -18

    lsw r5, -16
    rcl r5
    ssw r5, -16

    dec r4
  wend

  push r4
  push r4
  push r4
  push r4

  pop r4
  pop r5
  pop r6
  pop r7
  rts

end.