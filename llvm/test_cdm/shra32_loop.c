// CHECK reg(r0) 23421
// CHECK reg(r1) 65469

__attribute__((noinline)) long shra32_loop(long value, char amount) {
    return value >> amount;
}

long main(void) {
    const long value = 0xDEADBEEF;
    const char amount = 7;
    return shra32_loop(value, amount);
}
