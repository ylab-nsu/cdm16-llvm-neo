// CHECK reg(r0) 7

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b0000000100000000);
    return a;
}
