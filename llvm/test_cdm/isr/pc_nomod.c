// CHECK reg(r0) 10

#include <cdm/ivt.h>

#ifdef __COCAS__

int main() {
    return 10;
}

#else

volatile int add_value = 0;

static ISR void isr(context_t *context) {
    context->pc += add_value;
}

INTERRUPT_VECTORS(
    VECTOR(isr, 0)
);

int main() {
  register int value __asm__("r0");
  __asm__ volatile("ldi r0, 5\n\t"
                   "int %[vec]\n\t"
                   "inc r0\n\t"
                   "inc r0\n\t"
                   "inc r0\n\t"
                   "inc r0\n\t"
                   "inc r0\n\t"
                   : "=r"(value)
                   : [vec] "i"(EXCEPTION_COUNT));
  return value;
}

#endif
