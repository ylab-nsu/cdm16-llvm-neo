// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_uc((unsigned char) 0b11110101);
    return a;
}
