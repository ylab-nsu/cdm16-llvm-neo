// CHECK reg(r0) 5

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b00000101);
    return a;
}
