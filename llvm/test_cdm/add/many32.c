// CHECK reg(r0) 0x0C1C
// CHECK reg(r1) 0x9E85

__attribute__((noinline))
long add32(long a, long b, long c, long d, long e, long f) {
    return a + b + c + d + e + f;
}

long main() {
    volatile long a = 0x33453454;
    volatile long b = 0x00344535;
    volatile long c = 0x120ABC23;
    volatile long d = 0xDEADBEEF;
    volatile long e = 0x11111111;
    volatile long f = 0x69420670;
    return add32(a, b, c, d, e, f);
}
