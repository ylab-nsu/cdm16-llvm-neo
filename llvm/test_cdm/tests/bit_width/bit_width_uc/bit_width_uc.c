// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_width_uc((unsigned char) 0b00000000);
    return a;
}
