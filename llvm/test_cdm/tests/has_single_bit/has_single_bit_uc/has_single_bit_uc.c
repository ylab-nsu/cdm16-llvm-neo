// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    return stdc_has_single_bit_uc((unsigned char) 0b00010000);
}
