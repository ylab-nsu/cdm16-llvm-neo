// CHECK reg(r0) 11

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b00010101);
    return a;
}
