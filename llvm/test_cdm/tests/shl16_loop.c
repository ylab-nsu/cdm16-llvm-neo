// CHECK reg(r0) 64192

__attribute__((noinline)) int shl16_loop(int value, char amount) {
    return value << amount;
}

int main(void) {
    const int value = 0x0FAC;
    const char amount = 4;
    return shl16_loop(value, amount);
}
