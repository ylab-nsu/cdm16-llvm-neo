// CHECK reg(r0) 7

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b00000001);
    return a;
}
