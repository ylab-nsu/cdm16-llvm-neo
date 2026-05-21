// CHECK reg(r0) 7

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_width_uc((unsigned char) 0b01100100);
    return a;
}
