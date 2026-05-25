// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b01010101);
    return a;
}
