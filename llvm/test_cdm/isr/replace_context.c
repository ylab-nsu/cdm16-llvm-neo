// CHECK reg(r0) 1
// CHECK reg(r1) 2
// CHECK reg(r2) 3
// CHECK reg(r3) 4
// CHECK reg(r4) 5
// CHECK reg(r5) 6
// CHECK reg(r6) 7
// CHECK reg(fp) 8
// CHECK reg(ps) 9

#include <cdm/ivt.h>

#ifdef __COCAS__

__attribute__((naked)) int main() {
  asm("ldi r0, 1\n\t"
      "ldi r1, 2\n\t"
      "ldi r2, 3\n\t"
      "ldi r3, 4\n\t"
      "ldi r4, 5\n\t"
      "ldi r5, 6\n\t"
      "ldi r6, 7\n\t"
      "ldi r7, 9\n\t"
      "stps r7\n\t"
      "ldi r7, 8\n\t"
      "halt");
}

#else

void new_pc(void);

const context_t _context = {
    .r0 = 0,
    .r1 = 1,
    .r2 = 2,
    .r3 = 3,
    .r4 = 4,
    .r5 = 5,
    .r6 = 6,
    .fp = 7,
    .ps = 9,
    .pc = (reg_t)&new_pc,
};

ISR void isr(context_t *context) { *context = _context; }

INTERRUPT_VECTORS(VECTOR(isr, 0));

__attribute__((naked)) void new_pc() {
  asm("pups\n\t"
      "inc r0\n\t"
      "inc r1\n\t"
      "inc r2\n\t"
      "inc r3\n\t"
      "inc r4\n\t"
      "inc r5\n\t"
      "inc r6\n\t"
      "inc r7\n\t"
      "pops\n\t"
      "halt\n\t");
}

__attribute__((naked)) int main() {
  asm("ldi r0, 5\n\t"
      "ldi r1, 9\n\t"
      "ldi r2, 128\n\t"
      "ldi r3, 50\n\t"
      "ldi r4, 13\n\t"
      "ldi r5, 77\n\t"
      "ldi r6, 28\n\t"
      "ldi r7, 40\n\t"
      "stps r3\n\t"
      "int 0x10\n\t"
      "zero\n\t");
}

#endif
