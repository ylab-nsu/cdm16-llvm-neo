// CHECK reg(r0) 0b0000000000000000
// CHECK reg(r1) 0b0010000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_ul((unsigned long) 0b00011001000000101000010000010011);
    return a;
}
