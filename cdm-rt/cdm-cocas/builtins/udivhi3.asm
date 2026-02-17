rsect __udivhi3

__udivhi3>
  ldi r2, 0
  ldi r3, 16

  while
    tst r3
  stays nz
    shl r0
    rcl r2

    if
      cmp r2, r1
    is hs
      sub r2, r1, r2
      inc r0
    fi

    dec r3
  wend

  rts

end.
