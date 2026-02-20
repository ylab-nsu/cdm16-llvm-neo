rsect __mulhi3

__mulhi3>
  ldi r2, 0

  while
    tst r1
  stays nz
    if 
      shr r1
    is cs
      add r0, r2
    fi
    shl r0
  wend

  move r2, r0

  rts

end.