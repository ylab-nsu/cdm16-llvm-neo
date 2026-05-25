// CHECK reg(r0) 6

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_uc((unsigned char) 0b00000010);
    return a;
}
