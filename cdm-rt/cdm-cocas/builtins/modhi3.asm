rsect __modhi3

__umodhi3:ext

__modhi3>
  push r4
  move r0, r4

  if
    tst r0
  is mi
    neg r0
  fi

  if
    tst r1
  is mi
    neg r1
  fi
  
  jsr __umodhi3

  if
    tst r4
  is mi
    neg r0
  fi

  pop r4
  rts

end.