// CHECK reg(r0) 32768
// CHECK reg(r1) 2

__attribute__((noinline)) long shl32_const(long value) {
    return value << 2;
}

long main(void) {
    const long value = 0x00A000;
    return shl32_const(value);
}
