EXTERN(_default_handler);

PROVIDE(_exception_handler = _default_handler);
PROVIDE(_interrupt_handler = _default_handler);

PROVIDE(_ex_unaligned_sp = _exception_handler);
PROVIDE(_ex_unaligned_pc = _exception_handler);
PROVIDE(_ex_invalid_inst = _exception_handler);
PROVIDE(_ex_double_fault = _exception_handler);
PROVIDE(_ex_priv_violation = _exception_handler);
PROVIDE(_ex_reserved_6 = _exception_handler);
PROVIDE(_ex_syscall = _exception_handler);
PROVIDE(_ex_reserved_8 = _exception_handler);
PROVIDE(_ex_reserved_9 = _exception_handler);
PROVIDE(_ex_reserved_a = _exception_handler);
PROVIDE(_ex_reserved_b = _exception_handler);
PROVIDE(_ex_reserved_c = _exception_handler);
PROVIDE(_ex_reserved_d = _exception_handler);
PROVIDE(_ex_reserved_e = _exception_handler);
PROVIDE(_ex_reserved_f = _exception_handler);

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
  } > RAM

  .rodata : {
    *(.progmem*);           /* Program memory constants (for compatibility with Harvard model) */
    *(.rodata*);            /* Read-only data */
  } > RAM

  .data : {
    *(.data*);              /* Initialized variables */
  } > RAM

  .bss : {
    *(.bss*);               /* Uninitialized variables */
    *(COMMON);
  } > RAM

  __stack_start = (RAM_ORIGIN + RAM_LENGTH) & 0xFFFF;
}

ASSERT(RAM_ORIGIN % 2 == 0, "ERROR(cdm-rt): RAM_ORIGIN must be a multiple of 2");
ASSERT(RAM_ORIGIN % 2 != 0 || RAM_LENGTH % 2 == 0, "ERROR(cdm-rt): RAM_LENGTH must be a multiple of 2");
ASSERT(__vector_table_int != __vector_table, "ERROR(cdm-rt): missing exception vectors");
ASSERT(__vector_table_int == __vector_table || __vector_table_int == __vector_table + 16 * 0x4, "ERROR(cdm-rt): incorrect number of exception vectors (must be 16)");
