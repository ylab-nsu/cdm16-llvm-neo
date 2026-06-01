// CHECK reg(r0) 0x1111
// CHECK reg(r1) 0x9999

__attribute__((noinline))
long sub32(long a, long b) {
    return a - b;
}

long main() {
    volatile long a = 0xabcd1234;
    volatile long b = 0x12340123;
    return sub32(a, b);
}
