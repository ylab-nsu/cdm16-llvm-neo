OUTPUT_FORMAT("binary")

EXTERN(_default_handler);

PROVIDE(_exception_handler = _default_handler);
PROVIDE(_interrupt_handler = _default_handler);

PROVIDE(_unaligned_sp = _exception_handler);
PROVIDE(_unaligned_pc = _exception_handler);
PROVIDE(_invalid_inst = _exception_handler);
PROVIDE(_double_fault = _exception_handler);

MEMORY {
    IVT : ORIGIN = 0x0, LENGTH = 0x100
    RAM : ORIGIN = RAM_ORIGIN, LENGTH = RAM_LENGTH
}

PROVIDE(RAM_ORIGIN = 0x100);
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
  } > IVT

  .text : {
    *(.text._start);        /* Reset/startup code */
    *(.text*);              /* Program code */
    *(.rodata*);            /* Read-only data */
  } > RAM

  .data : {
    *(.data*);              /* Initialized variables */
  } > RAM

  .bss : {
    *(.bss*);               /* Uninitialized variables */
    *(COMMON);
  } > RAM
}

ASSERT(__vector_table_ex == __vector_table + 0x4, "ERROR(cdm-rt): missing reset vector");
ASSERT(__vector_table_ex == __vector_table || __vector_table_ex == __vector_table + 0x4, "ERROR(cdm-rt): invalid reset vector");
ASSERT(__vector_table_int != __vector_table_ex, "ERROR(cdm-rt): missing exception vectors");
ASSERT(__vector_table_int == __vector_table_ex || __vector_table_int == __vector_table_ex + 0x10, "ERROR(cdm-rt): incorrect number of exception vectors");
