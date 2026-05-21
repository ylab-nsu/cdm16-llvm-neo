// CHECK reg(r0) 0b00000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_uc((unsigned char) 0b11111111);
    return a;
}
