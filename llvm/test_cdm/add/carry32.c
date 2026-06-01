// CHECK reg(r0) 0x5323
// CHECK reg(r1) 0x3235

__attribute__((noinline))
long add32(long a, long b) {
    return a + b;
}

long main() {
    volatile long a = 0x1234fcab;
    volatile long b = 0x20005678;
    return add32(a, b);
}
