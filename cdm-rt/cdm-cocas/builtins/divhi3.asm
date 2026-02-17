rsect __divhi3

__udivhi3:ext

__divhi3>
  push r4
  ldi r4, 0

  if 
    tst r0
  is mi
    inc r4
    neg r0
  fi

  if
    tst r1
  is mi
    dec r4
    neg r1
  fi
  
  jsr __udivhi3

  if
    tst r4
  is nz
    neg r0
  fi

  pop r4
  rts

end.