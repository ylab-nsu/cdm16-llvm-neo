// CHECK reg(r0) 6

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b0000001000000000);
    return a;
}
