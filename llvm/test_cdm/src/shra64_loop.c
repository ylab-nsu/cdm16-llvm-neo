__attribute__((noinline)) long long shra64_loop(long long value, char amount) {
    return value >> amount;
}

long long main(void) {
    const long long value = 0x0FAC0BEBDEAD1EAA;
    const char amount = 17;
    return shra64_loop(value, amount);
}
