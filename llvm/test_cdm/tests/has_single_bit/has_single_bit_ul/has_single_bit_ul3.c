// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    return stdc_has_single_bit_ul((unsigned long) 0b00110001010000000000001000000000);
}
