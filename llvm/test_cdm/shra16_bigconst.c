// CHECK reg(r0) 1

__attribute__((noinline)) int shra16_bigconst(int value) {
    return value >> 10;
}

int main(void) {
    const int value = 1024;
    return shra16_bigconst(value);
}
