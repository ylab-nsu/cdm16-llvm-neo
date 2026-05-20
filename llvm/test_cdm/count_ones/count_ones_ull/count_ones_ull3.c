// CHECK reg(r0) 23

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_ull((unsigned long long) 0b0011100101001001000000111000000001010001110000100101000100010111);
    return a;
}

