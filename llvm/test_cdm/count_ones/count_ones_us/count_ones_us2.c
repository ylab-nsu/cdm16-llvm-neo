// CHECK reg(r0) 2

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_us((unsigned short) 0b0000110000000000);
    return a;
}
