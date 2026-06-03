EXTERN(_default_handler);

PROVIDE(_exception_handler = _default_handler);
PROVIDE(_interrupt_handler = _default_handler);

PROVIDE(_unaligned_sp = _exception_handler);
PROVIDE(_unaligned_pc = _exception_handler);
PROVIDE(_invalid_inst = _exception_handler);
PROVIDE(_double_fault = _exception_handler);

MEMORY {
    PROGMEM : ORIGIN = 0x0, LENGTH = 64K
    RAM : ORIGIN = 0x80000 + RAM_ORIGIN, LENGTH = RAM_LENGTH
}

PROVIDE(RAM_ORIGIN = 0x0);
PROVIDE(RAM_LENGTH = 64K-RAM_ORIGIN);

SECTIONS
{
  .ivt 0x00 : {             /* Interrupt vector table */
    __vector_table = .;
    KEEP(*(.ivt.reset_vector));
    __vector_table_ex = .;
    KEEP(*(.ivt.exceptions));
    __vector_table_int = .;
    KEEP(*(.ivt.interrupts));
    FILL(0xFE000000);       /* Point to two 0 bytes at the end of the IVT to trigger */
    . = 0x100;              /* an exception on unhandled interrupt */
    __vector_table_end = .;
  } > PROGMEM AT> PROGMEM

  .text : ALIGN(2) {
    *(.text._start);        /* Reset/startup code */
    *(.text*);              /* Program code */
  } > PROGMEM AT> PROGMEM

  .progmem : ALIGN(2) {
    *(.progmem*);           /* Program memory constants */
  } > PROGMEM AT> PROGMEM

  .data : ALIGN(2) {
    __data = .;
    *(.rodata*);            /* Read-only data */
    *(.data*);              /* Initialized variables */
    . = ALIGN(2);
  } > RAM AT> PROGMEM

  .bss : ALIGN(2) {
    __bss = .;
    *(.bss*);               /* Uninitialized variables */
    *(COMMON);
    . = ALIGN(2);
  } > RAM

  __data_rom = LOADADDR(.data);
  __data_length = SIZEOF(.data);
  __bss_length = SIZEOF(.bss);
  __stack_start = (RAM_ORIGIN + RAM_LENGTH) & 0xFFFF;
}

ASSERT(RAM_ORIGIN % 2 == 0, "ERROR(cdm-rt): RAM_ORIGIN must be a multiple of 2");
ASSERT(RAM_ORIGIN % 2 != 0 || RAM_LENGTH % 2 == 0, "ERROR(cdm-rt): RAM_LENGTH must be a multiple of 2");
ASSERT(__vector_table_ex == __vector_table + 0x4, "ERROR(cdm-rt): missing reset vector");
ASSERT(__vector_table_ex == __vector_table || __vector_table_ex == __vector_table + 0x4, "ERROR(cdm-rt): invalid reset vector");
ASSERT(__vector_table_int != __vector_table_ex, "ERROR(cdm-rt): missing exception vectors");
ASSERT(__vector_table_int == __vector_table_ex || __vector_table_int == __vector_table_ex + 0x10, "ERROR(cdm-rt): incorrect number of exception vectors");
