// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b1010111100000000);
    return a;
}
