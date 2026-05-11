// CHECK reg(r0) 0xFFFF
// CHECK reg(r1) 0xFFFF
// CHECK reg(r2) 0xFFFF
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long sub64(long long a, long long b) {
    return a - b;
}

long long main() {
    volatile long long a = 0x0001000000000000LL;
    volatile long long b = 0x0000000000000001LL;
    return sub64(a, b);
}