// CHECK reg(r0) 0b0000100000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_floor_us((unsigned short) 0b0000101110000100);
    return a;
}
