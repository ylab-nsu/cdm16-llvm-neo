__attribute__((noinline)) long shra32_const(long value) {
    return value >> 7;
}

long main(void) {
    const long value = 0xDEADBEEF;
    return shra32_const(value);
}
