// CHECK reg(r0) 0x0001
// CHECK reg(r1) 0x0000
// CHECK reg(r2) 0x0000
// CHECK reg(r3) 0x0001

__attribute__((noinline))
long long add64(long long a, long long b) {
    return a + b;
}

long long main() {
    volatile long long a = 0x0000FFFFFFFFFFFFLL;
    volatile long long b = 0x0000000000000002LL;
    return add64(a, b);
}
