// CHECK reg(r0) 0x7F81
// CHECK reg(r1) 0x001F

__attribute__((noinline))
unsigned long umodsi(unsigned long a, unsigned long b) {
    return a % b; 
}

unsigned long main() {
    volatile unsigned long a = 536870912;
    volatile unsigned long b = 2097281;
    return umodsi(a, b); 
}