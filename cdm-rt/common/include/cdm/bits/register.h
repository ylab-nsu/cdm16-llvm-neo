#if defined(__CDM__) && !defined(_CDM_RT_REGISTER_TYPES_H)
#define _CDM_RT_REGISTER_TYPES_H

// Processor register value type
typedef unsigned int reg_t;

// Status register bit field.
typedef enum : reg_t {
  // No flags.
  PS_NONE = 0x0,
  // Arithmetic negative flag.
  PS_NEGATIVE = 0x1,
  // Arithmetic zero flag.
  PS_ZERO = 0x2,
  // Arithmetic overflow flag.
  PS_OVERFLOW = 0x4,
  // Arithmetic carry flag.
  PS_CARRY = 0x8,
  // Interrupt enable flag.
  PS_INTERRUPT = 0x8000,
} psr_t;

// Processor context, excluding SP
typedef struct {
    reg_t r0;
    reg_t r1;
    reg_t r2;
    reg_t r3;
    reg_t r4;
    reg_t r5;
    reg_t r6;
    reg_t fp;
    reg_t pc;
    psr_t ps;
} context_t;

#endif
