// CHECK reg(r0) 6

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_ull((unsigned long long) 0b0000000000000110000000110000000000000001100000000000000000000000);
    return a;
}

