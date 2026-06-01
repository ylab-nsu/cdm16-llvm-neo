// CHECK reg(r0) 0x334F
// CHECK reg(r1) 0x0002

__attribute__((noinline))
long add32(long a, long b) {
    return a + b;
}

long main() {
    volatile long a = 0xFFFFF000;
    volatile long b = 0x0002434F;
    return add32(a, b);
}
