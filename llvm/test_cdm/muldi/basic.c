// CHECK reg(r0) 0x000F
// CHECK reg(r1) 0x0000
// CHECK reg(r2) 0x0000
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long mul64(long long a, long long b) {
    return a * b;
}

long long main() {
    volatile long long a = 3;
    volatile long long b = 5;
    return mul64(a, b);
}
