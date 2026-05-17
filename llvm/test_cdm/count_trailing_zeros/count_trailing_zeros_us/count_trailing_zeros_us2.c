// CHECK reg(r0) 6

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b0000000001000000);
    return a;
}
