// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b0000000010101111);
    return a;
}
