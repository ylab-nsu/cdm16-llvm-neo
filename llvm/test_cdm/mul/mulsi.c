// CHECK reg(r0) 511
// CHECK reg(r1) 65535

__attribute__((noinline))
long mulsi(long a, long b) {
    return a * b; 
}

long main() {
    volatile long a = -255;
    volatile long b = 255;
    return mulsi(a, b); 
}