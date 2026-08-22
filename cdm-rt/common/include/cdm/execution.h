#ifndef _CDM_RT_EXECUTION_H
#define _CDM_RT_EXECUTION_H

// Halt the processor until a hard reset is performed.
__attribute__((always_inline))
__attribute__((noreturn))
static inline void halt(void) {
  // Memory constraint to make sure that no writes are moved past
  // the halt instruction.
  __asm__ __volatile__("halt" : : : "memory");
  __builtin_unreachable();
}

// Perform a soft reset to interrupt vector 0.
__attribute__((always_inline))
__attribute__((noreturn))
static inline void reset(void) {
  // Same as "halt".
  __asm__ __volatile__("reset" : : : "memory");
  __builtin_unreachable();
}

#endif
