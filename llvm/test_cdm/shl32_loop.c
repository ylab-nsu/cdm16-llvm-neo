// CHECK reg(r0) 32768
// CHECK reg(r1) 2

__attribute__((noinline)) long shl32_loop(long value, char amount) {
    return value << amount;
}

long main(void) {
    const long value = 0x00A000;
    const char amount = 2;
    return shl32_loop(value, amount);
}
