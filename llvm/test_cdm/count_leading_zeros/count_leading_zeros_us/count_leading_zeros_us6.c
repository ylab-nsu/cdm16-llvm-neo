// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b0101010100000000);
    return a;
}
