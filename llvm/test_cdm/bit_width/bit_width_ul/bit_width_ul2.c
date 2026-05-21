// CHECK reg(r0) 21

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_width_ul((unsigned long) 0b00000000000100010101100000100000);
    return a;
}
