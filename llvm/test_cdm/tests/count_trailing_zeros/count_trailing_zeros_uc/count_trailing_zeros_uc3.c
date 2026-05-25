// CHECK reg(r0) 3

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_uc((unsigned char) 0b10101000);
    return a;
}
