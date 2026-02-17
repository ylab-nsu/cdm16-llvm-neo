rsect __modsi3

__umodsi3:ext

__modsi3>
  # a = r1:r0, b = r3:r2
  # q = r1:r0, r = r5:r4
  push r4
  ldi r4, 0

  if 
    tst r1
  is mi
    inc r4

    not r0
    inc r0
    not r1
    if 
      nop
    is cs
      inc r1
    fi
  fi

  if
    tst r3
  is mi
    dec r4

    not r2
    inc r2
    not r3
    if 
      nop
    is cs
      inc r3
    fi
  fi

  push r4

  jsr __umodsi3

  pop r4

  if
    tst r4
  is nz
    not r0
    not r1
    inc r0
    if
      nop
    is cs
      inc r1
    fi
  fi

  pop r4

  rts

end.