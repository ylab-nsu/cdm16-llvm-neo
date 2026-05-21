// CHECK reg(r0) 0xAA6E
// CHECK reg(r1) 0xE107
// CHECK reg(r2) 0x0003
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long div64(long long a, long long b) {
    return a / b;
}

long long main() {
    volatile long long a = 1152991873360597264LL;
    volatile long long b = 69206041LL;
    return div64(a, b);
}
