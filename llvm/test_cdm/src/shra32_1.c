__attribute__((noinline)) long shra32_1(long value) {
    return value >> 1;
}

long main(void) {
    const long value = 0xDEADBEEF;
    return shra32_1(value);
}
