// CHECK reg(r0) 8

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b00000000);
    return a;
}
