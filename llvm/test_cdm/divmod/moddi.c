// CHECK reg(r0) 0x6C52
// CHECK reg(r1) 0x0112
// CHECK reg(r2) 0x0000
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long umoddi(long long a, long long b) {
    return a % b;
}

long long main() {
    volatile long long a = 1152991873360597264;
    volatile long long b = 69206041;
    return umoddi(a, b);
}