// CHECK reg(r0) 0b0000000000000000
// CHECK reg(r1) 0b1000000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_floor_ul((unsigned long) 0b11111111111111111111111111111111);
    return a;
}
