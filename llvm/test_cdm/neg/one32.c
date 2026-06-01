// CHECK reg(r0) 0xFFFF
// CHECK reg(r1) 0xFFFF

__attribute__((noinline))
long neg32(long a) {
    return -a;
}

long main() {
    volatile long a = 1;
    return neg32(a);
}
