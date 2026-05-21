// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    return stdc_has_single_bit_ul((unsigned long) 0b00000000000100000000000000000000);
}
