asect 0x0
_start, _default_handler: ext

dc _start, 0
dc _default_handler, 0   # Unaligned SP
dc _default_handler, 0   # Unaligned PC
dc _default_handler, 0   # Invalid instruction
dc _default_handler, 0   # Double fault
align 0x80

rsect default_handler

_default_handler>
  halt

rsect start

main: ext

_start>
  jsr main
  halt

end.
