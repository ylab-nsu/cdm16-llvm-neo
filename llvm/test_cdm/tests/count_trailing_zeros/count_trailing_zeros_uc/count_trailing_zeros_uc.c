// CHECK reg(r0) 7

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_uc((unsigned char) 0b10000000);
    return a;
}
