__attribute__((noinline)) long long shl64_loop(long long value, char amount) {
    return value << amount;
}

long long main(void) {
    const long long value = 0x0FAC0BEBDEAD1EAA;
    const char amount = 13;
    return shl64_loop(value, amount);
}
