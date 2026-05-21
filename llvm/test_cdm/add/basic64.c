// CHECK reg(r0) 0xABBA
// CHECK reg(r1) 0xAAAA
// CHECK reg(r2) 0xBBBB
// CHECK reg(r3) 0xAAAA

__attribute__((noinline))
long long add64(long long a, long long b) {
    return a + b;
}

long long main() {
    volatile long long a = 0x1111222233334444;
    volatile long long b = 0x9999999977776776;
    return add64(a, b);
}
