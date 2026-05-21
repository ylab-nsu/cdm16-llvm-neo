// CHECK reg(r0) 0x7F81
// CHECK reg(r1) 0x001F

__attribute__((noinline))
long modsi(long a, long b) {
    return a % b; 
}

long main() {
    volatile long a = 536870912;
    volatile long b = 2097281;
    return modsi(a, b); 
}