.section .isr_vector, "a"

.short _start, 0
.short _cdm_default_handler_1, 0
.short _cdm_default_handler_2, 0
.short _cdm_default_handler_3, 0
.short _cdm_default_handler_4, 0

.text

.global _start, _cdm_default_handler_1, _cdm_default_handler_2, _cdm_default_handler_3, _cdm_default_handler_4

_cdm_default_handler_1:
  ldi r0, 0xDED1
  br _cdm_default_handlers_end
_cdm_default_handler_2:
  ldi r0, 0xDED2
  br _cdm_default_handlers_end
_cdm_default_handler_3:
  ldi r0, 0xDED3
  br _cdm_default_handlers_end
_cdm_default_handler_4:
  ldi r0, 0xDED4
_cdm_default_handlers_end:
  pop r1                  # Pop saved PC into r1
  halt

_start:
  jsr main
  halt

