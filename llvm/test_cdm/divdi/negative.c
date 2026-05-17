// CHECK reg(r0) 0xFFFD
// CHECK reg(r1) 0xFFFF
// CHECK reg(r2) 0xFFFF
// CHECK reg(r3) 0xFFFF

__attribute__((noinline))
long long div64(long long a, long long b) {
    return a / b;
}

long long main() {
    volatile long long a = -15;
    volatile long long b = 4;
    return div64(a, b);
}
