// CHECK reg(r0) 0xFFFE
// CHECK reg(r1) 0xFFFF
// CHECK reg(r2) 0xFFFF
// CHECK reg(r3) 0xFFFF

__attribute__((noinline))
long long sub64(long long a, long long b) {
    return a - b;
}

long long main() {
    volatile long long a = 3;
    volatile long long b = 5;
    return sub64(a, b);
}
