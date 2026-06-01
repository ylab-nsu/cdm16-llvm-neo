// CHECK reg(r0) 0x0000
// CHECK reg(r1) 0x0000

__attribute__((noinline))
long neg32(long a) {
    return -a;
}

long main() {
    volatile long a = 0;
    return neg32(a);
}
