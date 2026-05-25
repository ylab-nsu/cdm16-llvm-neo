// CHECK reg(r0) 0b0000000000000001

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_us((unsigned short) 0b0000000000000000);
    return a;
}
