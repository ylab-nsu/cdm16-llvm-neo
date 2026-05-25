// CHECK reg(r0) 0b1000000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_floor_us((unsigned short) 0b1111111111111111);
    return a;
}
