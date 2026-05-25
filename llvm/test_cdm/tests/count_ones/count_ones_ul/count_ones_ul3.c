// CHECK reg(r0) 15

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_ul((unsigned long) 0b00011101110000101101011101100000);
    return a;
}
