#if defined(__CDM__) && !defined(_CDM_RT_INTERRUPT_H)
#define _CDM_RT_INTERRUPT_H

// Enables all interrupts.
static inline void int_enable() {
  // Memory constraint is to prevent the compiler
  // from reordering loads/stores with the asm block.
  // Basically, this is a compiler fence.
  __asm__ __volatile__("ei" : : : "memory", "cc");
}

// Disables all interrupts.
static inline void int_disable() {
  // Same as "ei"
  __asm__ __volatile__("di" : : : "memory", "cc");
}

// Waits for an interrupt.
static inline void int_wait() {
  // Fence is to make sure that all writes are not moved past the wait
  // instruction.
  __asm__ __volatile__("wait" : : : "memory");
}

// Triggers a software interrupt with the specified vector.
// vector must be in the range [0; 63].
#define int_trigger(vector)                                                    \
  do {                                                                         \
    _Static_assert((vector) >= 0 && (vector) <= 63,                            \
                   "Interrupt vector out of range");                           \
    __asm__ __volatile__("int %[vec]" : : [vec] "i"(vector) : "memory", "cc"); \
  } while (0)

#endif
