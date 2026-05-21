// CHECK reg(r0) 17

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_ul((unsigned long) 0b01101011101011100000000000000000);
    return a;
}
