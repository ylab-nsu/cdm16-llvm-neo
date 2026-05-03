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

// Reads from the status register.
static inline psr_t ldps() {
  psr_t value;
  __asm__ __volatile__("ldps %[val]" : [val] "=r"(value));
  return value;
}

// Writes to the status register.
static inline void stps(psr_t value) {
  // Memory constraint is to prevent the compiler
  // from reordering loads/stores with the asm block.
  // Basically, this is a compiler fence.
  __asm__ __volatile__("stps %[val]" : : [val] "r"(value) : "memory");
}

// Reads from the program counter register.
static inline void *ldpc() {
  void *value;
  __asm__ __volatile__("ldpc %[val]" : [val] "=r"(value));
  return value;
}

// Reads from the stack pointer register.
static inline void *ldsp() {
  void *value;
  __asm__ __volatile__("ldsp %[val]" : [val] "=r"(value));
  return value;
}

// Reads from the frame pointer register.
static inline void *ldfp() {
  void *value;
  __asm__ __volatile__("move fp, %[val]" : [val] "=r"(value) : : "cc");
  return value;
}

#endif
