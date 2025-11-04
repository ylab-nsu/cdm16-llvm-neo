__attribute__((noinline)) long long shra64_1(long long value) {
    return value >> 1;
}

long long main(void) {
    const long long value = 0x0FAC0BEBDEAD1EAA;
    return shra64_1(value);
}
