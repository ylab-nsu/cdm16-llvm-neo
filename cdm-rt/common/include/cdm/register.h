#if defined(__CDM__) && !defined(_CDM_RT_REGISTER_H)
#define _CDM_RT_REGISTER_H

// Status register bit field.
typedef enum : unsigned int {
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

// Read from the status register.
__attribute__((always_inline))
static inline psr_t ldps(void) {
  psr_t value;
  __asm__ __volatile__("ldps %[val]" : [val] "=r"(value));
  return value;
}

// Write to the status register.
__attribute__((always_inline))
static inline void stps(psr_t value) {
  // Memory constraint is to prevent the compiler
  // from reordering loads/stores with the asm block.
  __asm__ __volatile__("stps %[val]" : : [val] "r"(value) : "memory");
}

// Read from the program counter register.
__attribute__((always_inline))
static inline void *ldpc(void) {
  void *value;
  __asm__ __volatile__("ldpc %[val]" : [val] "=r"(value));
  return value;
}

// Read from the stack pointer register.
__attribute__((always_inline))
static inline void *ldsp(void) {
  void *value;
  __asm__ __volatile__("ldsp %[val]" : [val] "=r"(value));
  return value;
}

// Read from the frame pointer register.
__attribute__((always_inline))
static inline void *ldfp(void) {
  void *value;
  __asm__ __volatile__("move fp, %[val]" : [val] "=r"(value) : : "cc");
  return value;
}

#endif
