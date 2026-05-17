// CHECK reg(r0) 0x0002
// CHECK reg(r1) 0x0000
// CHECK reg(r2) 0x0000
// CHECK reg(r3) 0x0000

__attribute__((noinline))
long long mod64(long long a, long long b) {
    return a % b;
}

long long main() {
    volatile long long a = 100;
    volatile long long b = 7;
    return mod64(a, b);
}
