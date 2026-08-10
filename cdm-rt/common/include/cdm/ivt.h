#if defined(__CDM__) && !defined(_CDM_RT_IVT_H)
#define _CDM_RT_IVT_H

// Register types
#include <cdm/bits/register.h>

// Makes the function an interrupt service routine.
#define ISR __attribute__((interrupt))

// Interrupt service routine type.
typedef ISR void isr_t(context_t *context);

// IVT entry.
typedef struct {
    // The ISR pointer.
    isr_t *handler;
    // The value of PSR when the ISR is invoked.
    unsigned int psr;
} ivt_vector_t;

// The default interrupt service routine.
isr_t _interrupt_handler;

// Defines an IVT entry.
#define VECTOR(handler, psr) ((ivt_vector_t){ (isr_t*)(handler), (psr) })
// The default interrupt vector for unused interrupts.
#define DEFAULT_VECTOR ((ivt_vector_t){ _interrupt_handler, 0 })

// The number of standard exception handler entries (including the reset vector) in the IVT.
#define EXCEPTION_COUNT 16
// The number of user interrupt handler entries in the IVT.
#define INTERRUPT_COUNT 48

#ifndef __COCAS__
#define __CDM_RT_IVT_COUNT(...) __CDM_RT_IVT_COUNT_(__VA_ARGS__ __VA_OPT__(,)  \
    65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50,            \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34,            \
    33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18,            \
    17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define __CDM_RT_IVT_COUNT_(                                                   \
    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13,                \
    _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25,                \
    _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37,                \
    _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49,                \
    _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61,                \
    _62, _63, _64, N, ...) N

// Defines the user interrupt handler section of the IVT.
#define INTERRUPT_VECTORS(...)                                                 \
    __attribute__((used, section(".ivt.interrupts")))                          \
    static const ivt_vector_t __INTERRUPTS[INTERRUPT_COUNT] = {                \
        __VA_ARGS__ __VA_OPT__(,)                                              \
        [__CDM_RT_IVT_COUNT(__VA_ARGS__)                                       \
            ...(INTERRUPT_COUNT - 1)] = DEFAULT_VECTOR                         \
    }
#else
#define INTERRUPT_VECTORS(...)                                                 \
    _Static_assert(0, "INTERRUPT_VECTORS is not supported on cdm-cocas; define your IVT in an assembly file instead")
#endif

#endif
