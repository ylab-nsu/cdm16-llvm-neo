// CHECK reg(r0) 15

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b1000000000000000);
    return a;
}
