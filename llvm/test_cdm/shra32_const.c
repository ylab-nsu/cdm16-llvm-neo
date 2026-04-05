// CHECK reg(r0) 23421
// CHECK reg(r1) 65469

__attribute__((noinline)) long shra32_const(long value) {
    return value >> 7;
}

long main(void) {
    const long value = 0xDEADBEEF;
    return shra32_const(value);
}
