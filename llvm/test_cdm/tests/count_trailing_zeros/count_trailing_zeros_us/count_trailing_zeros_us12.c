// CHECK reg(r0) 11

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b1010100000000000);
    return a;
}
