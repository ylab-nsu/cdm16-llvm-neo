// CHECK reg(r0) 0x1219
// CHECK reg(r1) 0x8000
// CHECK reg(r2) 1

__attribute__((noinline))
long long mulosi(long a, long b) {
    long result;
    int overflow = 0;
    if (__builtin_mul_overflow(a, b, &result)) {
        overflow = 1;
    }
    return ((long long)overflow << 32) | (unsigned long)result;
}

long long main() {
    volatile long a = 0xB505;
    volatile long b = 0xB505;
    return mulosi(a, b);
}
