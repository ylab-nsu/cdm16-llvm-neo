// CHECK reg(r0) 16

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b00000000);
    return a;
}
