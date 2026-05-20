// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    return stdc_has_single_bit_uc((unsigned char) 0b10011100);
}
