// CHECK reg(r0) 0b0000000000000000
// CHECK reg(r1) 0b0000000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_ul((unsigned long) 0b11111111111111111111111111111111);
    return a;
}
