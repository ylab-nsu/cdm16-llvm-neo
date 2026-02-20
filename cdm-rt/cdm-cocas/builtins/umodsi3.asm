rsect __umodsi3

__umodsi3>
  # a = r1:r0, b = r3:r2
  # q = r1:r0, r = r5:r4
  push r6

  ldi r4, 0
  ldi r5, 0
  ldi r6, 32

  while
    tst r6
  stays nz
    shl r0
    rcl r1
    rcl r4
    rcl r5

    if
      cmp r5, r3
    is hi
      sub r5, r3, r5
      subc r4, r2, r4
      inc r0
    else
      if
        cmp r5, r3
      is eq
        if
          cmp r4, r2
        is hs
          sub r5, r3, r5
          subc r4, r2, r4
          inc r0
        fi
      fi
    fi

    dec r6
  wend

  move r4, r0
  move r5, r1

  pop r6

  rts

end.