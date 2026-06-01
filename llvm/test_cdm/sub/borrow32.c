// CHECK reg(r0) 0xbbbc
// CHECK reg(r1) 0x9998

__attribute__((noinline))
long sub32(long a, long b) {
    return a - b;
}

long main() {
    volatile long a = 0xabcd1234;
    volatile long b = 0x12345678;
    return sub32(a, b);
}
