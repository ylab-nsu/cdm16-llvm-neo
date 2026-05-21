// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_uc((unsigned char) 0b10101010);
    return a;
}
