// CHECK reg(r0) 0x579c
// CHECK reg(r1) 0xacf0

__attribute__((noinline))
long sub32(long a, long b) {
    return a - b;
}

long main() {
    volatile long a = 0xabcd1234;
    volatile long b = 0xfedcba98;
    return sub32(a, b);
}
