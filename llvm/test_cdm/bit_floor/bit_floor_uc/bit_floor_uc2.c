// CHECK reg(r0) 0b00100000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_floor_uc((unsigned char) 0b00110010);
    return a;
}
