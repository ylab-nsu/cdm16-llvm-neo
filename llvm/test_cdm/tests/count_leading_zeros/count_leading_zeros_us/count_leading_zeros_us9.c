// CHECK reg(r0) 15

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b00000001);
    return a;
}
