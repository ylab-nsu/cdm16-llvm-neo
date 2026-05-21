// CHECK reg(r0) 0x6C52
// CHECK reg(r1) 0x0112
// CHECK reg(r2) 0x0000
// CHECK reg(r3) 0x0000

__attribute__((noinline))
unsigned long long umoddi(unsigned long long a, unsigned long long b) {
    return a % b;
}

unsigned long long main() {
    volatile unsigned long long a = 1152991873360597264;
    volatile unsigned long long b = 69206041;
    return umoddi(a, b);
}
