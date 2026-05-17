// CHECK reg(r0) 13

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b1010000000000000);
    return a;
}
