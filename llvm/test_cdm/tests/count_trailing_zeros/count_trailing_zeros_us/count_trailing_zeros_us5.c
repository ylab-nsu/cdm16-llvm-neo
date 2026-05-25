// CHECK reg(r0) 2

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b0000000011010100);
    return a;
}
