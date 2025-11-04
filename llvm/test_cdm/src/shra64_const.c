__attribute__((noinline)) long long shra64_const(long long value) {
    return value >> 17;
}

long long main(void) {
    const long long value = 0x0FAC0BEBDEAD1EAA;
    return shra64_const(value);
}
