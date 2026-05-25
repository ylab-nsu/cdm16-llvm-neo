// CHECK reg(r0) 9

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b1010101000000000);
    return a;
}
