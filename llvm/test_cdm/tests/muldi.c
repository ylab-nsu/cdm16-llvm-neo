// CHECK reg(r0) 0xA2D2
// CHECK reg(r1) 0x431F
// CHECK reg(r2) 0xEEAE
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long muldi(long long a, long long b) {
    return a * b; 
}

long long main() {
    volatile long long a = 19999221;
    volatile long long b = 13122122;
    return muldi(a, b); 
}