// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b10101111);
    return a;
}
