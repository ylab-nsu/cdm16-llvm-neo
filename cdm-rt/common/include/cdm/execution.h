#if defined(__CDM__) && !defined(_CDM_RT_EXECUTION_H)
#define _CDM_RT_EXECUTION_H

// Stops the clock, putting the processor into the HALTED state.
__attribute__((noreturn)) static inline void halt() {
  // Memory constraint to make sure that no writes are moved past
  // the halt instruction.
  __asm__ __volatile__("halt" : : : "memory");
  __builtin_unreachable();
}

// Performs a soft reset, fetching interrupt vector 0.
__attribute__((noreturn)) static inline void reset() {
  // Same as "halt".
  __asm__ __volatile__("reset" : : : "memory");
  __builtin_unreachable();
}

#endif
