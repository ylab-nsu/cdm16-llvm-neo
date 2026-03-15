rsect __modhi3

__umodhi3:ext

__modhi3>
  push r4
  ldi r4, 0
  
  tst r0
  bpl __modhi3_r0_pos
  inc r4
  neg r0
__modhi3_r0_pos:
  tst r1
  bpl __modhi3_r1_pos
  dec r4
  neg r1
__modhi3_r1_pos:
  jsr __umodhi3
  tst r4
  bz __modhi3_done
  neg r0
__modhi3_done:
  pop r4
  rts

end.