// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_width_ul((unsigned long) 0b00000000000000000000000000000000);
    return a;
}
