// CHECK reg(r0) 65004

__attribute__((noinline)) int shra16_const(int value) {
    return value >> 3;
}

int main(void) {
    const int value = -4256;
    return shra16_const(value);
}
