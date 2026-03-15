// CHECK reg(r0) 0xAA6E
// CHECK reg(r1) 0xE107
// CHECK reg(r2) 0x0003
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long udivdi(long long a, long long b) {
    return a / b;
}

long long main() {
    volatile long long a = 1152991873360597264;
    volatile long long b = 69206041;
    return udivdi(a, b);
}