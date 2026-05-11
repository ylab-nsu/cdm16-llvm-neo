// CHECK reg(r0) 0x0001
// CHECK reg(r1) 0x0000
// CHECK reg(r2) 0x0000
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long neg64(long long a) {
    return -a;
}

long long main() {
    volatile long long a = -1;
    return neg64(a);
}
