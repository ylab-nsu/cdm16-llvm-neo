// CHECK reg(r0) 4

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_ul((unsigned long) 0b00000000000000000000000011001100);
    return a;
}
