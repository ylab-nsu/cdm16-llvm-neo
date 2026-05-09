#if defined(__CDM__) && !defined(_CDM_RT_INTERRUPT_H)
#define _CDM_RT_INTERRUPT_H

// Enable hardware interrupts.
__attribute__((always_inline))
static inline void int_enable(void) {
  // Memory constraint is to prevent the compiler
  // from reordering loads/stores with the asm block.
  // Basically, this is a compiler fence.
  __asm__ __volatile__("ei" : : : "memory", "cc");
}

// Disable hardware interrupts.
__attribute__((always_inline))
static inline void int_disable(void) {
  // Same as "ei"
  __asm__ __volatile__("di" : : : "memory", "cc");
}

// Wait for a hardware interrupt.
__attribute__((always_inline))
static inline void int_wait(void) {
  // Memory constraint is to make sure that no writes
  // are moved past the wait instruction.
  __asm__ __volatile__("wait" : : : "memory");
}

// Trigger a software interrupt with the specified vector.
// Interrupt vector must be in the range [0; 63].
#define int_trigger(vector)                                                    \
  ({                                                                           \
    _Static_assert(__builtin_constant_p(vector),                               \
                   "interrupt vector must be constant");                       \
    _Static_assert(__builtin_choose_expr(__builtin_constant_p(vector),         \
                                         (vector) >= 0 && (vector) <= 63, 1),  \
                   "interrupt vector must be in the range [0; 63]");           \
    __asm__ __volatile__("int %[vec]" : : [vec] "i"(vector) : "memory", "cc"); \
  })

#endif
