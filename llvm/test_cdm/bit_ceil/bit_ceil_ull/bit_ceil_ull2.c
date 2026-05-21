// CHECK reg(r0) 0b0000000000000000
// CHECK reg(r1) 0b0000000000000000
// CHECK reg(r2) 0b0000000000000000
// CHECK reg(r3) 0b0001000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_ull((unsigned long long) 0b0000101011111000000000111010010000000000000110110000000000001110);
    return a;
}
