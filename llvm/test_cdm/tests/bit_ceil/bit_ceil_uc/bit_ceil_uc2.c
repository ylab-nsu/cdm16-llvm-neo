// CHECK reg(r0) 0b00100000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_uc((unsigned char) 0b00100000);
    return a;
}
