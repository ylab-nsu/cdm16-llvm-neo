#ifndef _CDM_RT_REGISTER_H
#define _CDM_RT_REGISTER_H

// Register types
#include <cdm/bits/register.h>

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
static inline reg_t ldpc(void) {
  reg_t value;
  __asm__ __volatile__("ldpc %[val]" : [val] "=r"(value));
  return value;
}

// Read from the stack pointer register.
__attribute__((always_inline))
static inline reg_t ldsp(void) {
  reg_t value;
  __asm__ __volatile__("ldsp %[val]" : [val] "=r"(value));
  return value;
}

// Read from the frame pointer register.
__attribute__((always_inline))
static inline reg_t ldfp(void) {
  reg_t value;
  __asm__ __volatile__("move fp, %[val]" : [val] "=r"(value) : : "cc");
  return value;
}

#endif
