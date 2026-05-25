// CHECK reg(r0) 2

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b00110101);
    return a;
}
