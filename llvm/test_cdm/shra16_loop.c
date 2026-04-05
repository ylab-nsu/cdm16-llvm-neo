// CHECK reg(r0) 65004

__attribute__((noinline)) int shra16_loop(int value, char amount) {
    return value >> amount;
}

int main(void) {
    const int value = -4256;
    const char amount = 3;
    return shra16_loop(value, amount);
}
