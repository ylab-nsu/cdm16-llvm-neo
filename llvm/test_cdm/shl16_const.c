// CHECK reg(r0) 64192

__attribute__((noinline)) int shl16_const(int value) {
    return value << 4;
}

int main(void) {
    const int value = 0x0FAC;
    return shl16_const(value);
}
