asect 0x0
_start, _cdm_default_handler_1, _cdm_default_handler_2, _cdm_default_handler_3, _cdm_default_handler_4: ext

dc _start, 0
dc _cdm_default_handler_1, 0   # Unaligned SP
dc _cdm_default_handler_2, 0   # Unaligned PC
dc _cdm_default_handler_3, 0   # Invalid instruction
dc _cdm_default_handler_4, 0   # Double fault

rsect _cdm_crt0

main: ext

_cdm_default_handler_1>
  ldi r0, 0xDED1
  br _cdm_default_handlers_end
_cdm_default_handler_2>
  ldi r0, 0xDED2
  br _cdm_default_handlers_end
_cdm_default_handler_3>
  ldi r0, 0xDED3
  br _cdm_default_handlers_end
_cdm_default_handler_4>
  ldi r0, 0xDED4
_cdm_default_handlers_end:
  pop r1                  # Pop saved PC into r1
  halt

_start>
  jsr main
  halt

end.
