// CHECK reg(r0) 0x20cA
// CHECK reg(r1) 0xfffd

__attribute__((noinline))
long sub32(long a, long b, long c, long d, long e, long f) {
    return a - b - c - d - e - f;
}

long main() {
    volatile long a = 100000;
    volatile long b = 4354;
    volatile long c = -140;
    volatile long d = -70000;
    volatile long e = 54000;
    volatile long f = 300000;
    return sub32(a, b, c, d, e, f);
}
