// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_us((unsigned short) 0b0000000010101010);
    return a;
}
