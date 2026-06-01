// CHECK reg(r0) 0x5723
// CHECK reg(r1) 0x3234

__attribute__((noinline))
long add32(long a, long b) {
    return a + b;
}

long main() {
    volatile long a = 0x123400ab;
    volatile long b = 0x20005678;
    return add32(a, b);
}
