// CHECK reg(r0) 10

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_us((unsigned short) 0b1011101111010001);
    return a;
}
