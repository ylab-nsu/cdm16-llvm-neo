asect 0x0
_start, _default_handler1, _default_handler2, _default_handler3, _default_handler4: ext

dc _start, 0
dc _default_handler1, 0   # Unaligned SP
dc _default_handler2, 0   # Unaligned PC
dc _default_handler3, 0   # Invalid instruction
dc _default_handler4, 0   # Double fault
align 0x80

rsect default_handler

_default_handler1>
  ldi r0, 0xDED1
  br _default_handlers_end
_default_handler2>
  ldi r0, 0xDED2
  br _default_handlers_end
_default_handler3>
  ldi r0, 0xDED3
  br _default_handlers_end
_default_handler4>
  ldi r0, 0xDED4
_default_handlers_end:
  pop r1                  # Pop saved PC into r1
  halt

rsect start

main: ext

_start>
  jsr main
  halt

end.
