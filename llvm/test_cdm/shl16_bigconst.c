// CHECK reg(r0) 1024

__attribute__((noinline)) int shl16_bigconst(int value) {
    return value << 10;
}

int main(void) {
    const int value = 1;
    return shl16_bigconst(value);
}
