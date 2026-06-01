// CHECK reg(r0) 0x128b
// CHECK reg(r1) 0xff96

__attribute__((noinline))
long neg32(long a) {
    return -a;
}

long main() {
    volatile long a = 6942069;
    return neg32(a);
}
