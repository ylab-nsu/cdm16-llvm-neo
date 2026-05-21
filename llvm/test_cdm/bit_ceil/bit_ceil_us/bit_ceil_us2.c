// CHECK reg(r0) 0b1000000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_us((unsigned short) 0b0110001010000100);
    return a;
}
