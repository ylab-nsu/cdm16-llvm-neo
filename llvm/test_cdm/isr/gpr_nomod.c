// CHECK reg(r0) 5

#include <cdm/ivt.h>

#ifdef __COCAS__

int main() {
    return 5;
}

#else

volatile int add_value = 0;

ISR void isr(context_t *context) {
    context->r0 += add_value;
}

INTERRUPT_VECTORS(
    VECTOR(isr, 0)
);

int main() {
  register int value __asm__("r0");
  __asm__ volatile("ldi r0, 5\n\t"
                   "int %[vec]\n\t"
                   : "=r"(value)
                   : [vec] "i"(EXCEPTION_COUNT));
  return value;
}

#endif
