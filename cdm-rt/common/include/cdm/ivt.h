#if defined(__CDM__) && !defined(_CDM_RT_IVT_H)
#define _CDM_RT_IVT_H

// Makes the function an interrupt service routine.
#define ISR __attribute__((CDM_ISR))

#endif
