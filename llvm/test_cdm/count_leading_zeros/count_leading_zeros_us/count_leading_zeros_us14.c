// CHECK reg(r0) 9

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b01010101);
    return a;
}
