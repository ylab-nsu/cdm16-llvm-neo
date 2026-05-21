// CHECK reg(r0) 3

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b00010101);
    return a;
}
