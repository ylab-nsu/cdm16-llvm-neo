// CHECK reg(r0) 2

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b0011010100000000);
    return a;
}
