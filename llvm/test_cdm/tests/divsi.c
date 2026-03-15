// CHECK reg(r0) 0x00FF
// CHECK reg(r1) 0x0000

__attribute__((noinline))
long divsi(long a, long b) {
    return a / b; 
}

long main() {
    volatile long a = 536870912;
    volatile long b = 2097281;
    return divsi(a, b); 
}