// CHECK reg(r0) 36693
// CHECK reg(r1) 61270
// CHECK reg(r2) 1525
// CHECK reg(r3) 2006

__attribute__((noinline)) long long shra64_1(long long value) {
    return value >> 1;
}

long long main(void) {
    const long long value = 0x0FAC0BEBDEAD1EAA;
    return shra64_1(value);
}
