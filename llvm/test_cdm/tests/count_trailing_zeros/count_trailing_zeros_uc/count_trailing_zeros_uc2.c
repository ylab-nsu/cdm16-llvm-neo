// CHECK reg(r0) 6

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_uc((unsigned char) 0b01000000);
    return a;
}
