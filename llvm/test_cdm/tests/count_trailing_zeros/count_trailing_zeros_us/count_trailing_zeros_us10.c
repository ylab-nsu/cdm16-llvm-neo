// CHECK reg(r0) 14

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b0100000000000000);
    return a;
}
