// CHECK reg(r0) 0xFFFF
// CHECK reg(r1) 0xFFFF
// CHECK reg(r2) 0xFFFF
// CHECK reg(r3) 0xFFFF

__attribute__((noinline))
long long neg64(long long a) {
    return -a;
}

long long main() {
    volatile long long a = 1;
    return neg64(a);
}
