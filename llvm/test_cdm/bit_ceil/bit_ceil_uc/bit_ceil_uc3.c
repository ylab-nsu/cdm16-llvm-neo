// CHECK reg(r0) 0b10000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_uc((unsigned char) 0b01010100);
    return a;
}
