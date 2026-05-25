// CHECK reg(r0) 5

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b0000010100000000);
    return a;
}
