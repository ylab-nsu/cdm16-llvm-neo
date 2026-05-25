// CHECK reg(r0) 0b0000100000000000
// CHECK reg(r1) 0b0000000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_floor_ul((unsigned long) 0b00000000000000000000110010100000);
    return a;
}
