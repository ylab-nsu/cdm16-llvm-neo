// CHECK reg(r0) 0x9999
// CHECK reg(r1) 0x9999
// CHECK reg(r2) 0x9999
// CHECK reg(r3) 0x9999

__attribute__((noinline))
long long sub64(long long a, long long b) {
    return a - b;
}

long long main() {
    volatile long long a = 0xAAAABBBBCCCCDDDD;
    volatile long long b = 0x1111222233334444;
    return sub64(a, b);
}
