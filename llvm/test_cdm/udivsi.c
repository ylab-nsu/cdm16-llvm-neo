// CHECK reg(r0) 0x00FF
// CHECK reg(r1) 0x0000

__attribute__((noinline))
unsigned long udivsi(unsigned long a, unsigned long b) {
    return a / b; 
}

unsigned long main() {
    volatile unsigned long a = 536870912;
    volatile unsigned long b = 2097281;
    return udivsi(a, b); 
}