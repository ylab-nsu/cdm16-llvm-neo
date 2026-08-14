// CHECK reg(r0) 5
// CHECK reg(r1) 9
// CHECK reg(r2) 128
// CHECK reg(r3) 50
// CHECK reg(r4) 13
// CHECK reg(r5) 77
// CHECK reg(r6) 28
// CHECK reg(fp) 40
// CHECK reg(sp) 128
// CHECK reg(ps) 50

#include <cdm/ivt.h>

#ifdef __COCAS__

__attribute__((naked)) int main() {
  asm("ldi r0, 5\n\t"
      "ldi r1, 9\n\t"
      "ldi r2, 128\n\t"
      "ldi r3, 50\n\t"
      "ldi r4, 13\n\t"
      "ldi r5, 77\n\t"
      "ldi r6, 28\n\t"
      "ldi r7, 40\n\t"
      "stsp r2\n\t"
      "stps r3\n\t"
      "halt");
}

#else

volatile long long value1 = 0x0FAC0BEBDEAD1EAA;
volatile long long value2 = 0x0FAC0BEBDEAD1EAA;

volatile context_t _context;

ISR void isr(context_t *context) {
  value1 <<= 30;
  value1 += value2;
}

INTERRUPT_VECTORS(VECTOR(isr, 0));

__attribute__((naked)) int main() {
  asm("ldi r0, 5\n\t"
      "ldi r1, 9\n\t"
      "ldi r2, 128\n\t"
      "ldi r3, 50\n\t"
      "ldi r4, 13\n\t"
      "ldi r5, 77\n\t"
      "ldi r6, 28\n\t"
      "ldi r7, 40\n\t"
      "stsp r2\n\t"
      "stps r3\n\t"
      "int 0x10\n\t"
      "halt");
}

#endif
