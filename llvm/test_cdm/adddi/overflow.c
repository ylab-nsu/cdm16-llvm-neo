// CHECK reg(r0) 0x0000
// CHECK reg(r1) 0x0000
// CHECK reg(r2) 0x0000
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long add64(long long a, long long b) {
    return a + b;
}

long long main() {
    volatile long long a = -1;
    volatile long long b = 1;
    return add64(a, b);
}
