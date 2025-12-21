// CHECK reg(r0) 16384
// CHECK reg(r1) 41941
// CHECK reg(r2) 31701
// CHECK reg(r3) 33149

__attribute__((noinline)) long long shl64_loop(long long value, char amount) {
    return value << amount;
}

long long main(void) {
    const long long value = 0x0FAC0BEBDEAD1EAA;
    const char amount = 13;
    return shl64_loop(value, amount);
}
