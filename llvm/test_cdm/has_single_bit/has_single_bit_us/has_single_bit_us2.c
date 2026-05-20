// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    return stdc_has_single_bit_us((unsigned short) 0b0000000000000000);
}
