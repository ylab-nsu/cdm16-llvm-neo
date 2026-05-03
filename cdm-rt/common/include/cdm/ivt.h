#if defined(__CDM__) && !defined(_CDM_RT_IVT_H)
#define _CDM_RT_IVT_H

// Makes the function an entry point.
#define cdm_entry __attribute__((noreturn))

// Makes the function an interrupt service routine.
#define cdm_interrupt __attribute__((CDM_ISR))

#endif
