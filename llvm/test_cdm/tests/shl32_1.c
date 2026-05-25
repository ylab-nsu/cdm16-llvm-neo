// CHECK reg(r0) 32222
// CHECK reg(r1) 48475

__attribute__((noinline)) long shl32_1(long value) {
    return value << 1;
}

long main(void) {
    const long value = 0xDEADBEEF;
    return shl32_1(value);
}
