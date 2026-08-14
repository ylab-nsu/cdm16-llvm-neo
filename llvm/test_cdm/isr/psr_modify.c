// CHECK reg(r0) 103

#include <cdm/ivt.h>

#ifdef __COCAS__

int main() {
    return 103;
}

#else

volatile int and_value = 0;

ISR void isr(context_t *context) {
    context->ps &= and_value;
}

INTERRUPT_VECTORS(
    VECTOR(isr, 0)
);

int main() {
  register int value __asm__("r0");
  __asm__ volatile("ldi r0, -5\n\t"
                   "cmp r0, 0\n\t"
                   "int %[vec]\n\t"
                   "blt _main%=\n\t"
                   "ldi r0, 0x67\n\t"
                   "_main%=:\n\t"
                   : "=r"(value)
                   : [vec] "i"(EXCEPTION_COUNT));
  return value;
}

#endif
