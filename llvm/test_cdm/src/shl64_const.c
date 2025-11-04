__attribute__((noinline)) long long shl64_const(long long value) {
    return value << 13;
}

long long main(void) {
    const long long value = 0x0FAC0BEBDEAD1EAA;
    return shl64_const(value);
}
