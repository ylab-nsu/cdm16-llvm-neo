// CHECK reg(r0) 5

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_ul((unsigned long) 0b01101011101011101101111010100000);
    return a;
}
