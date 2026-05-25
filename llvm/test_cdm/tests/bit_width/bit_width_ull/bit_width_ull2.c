// CHECK reg(r0) 35

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_width_ull((unsigned long long) 0b0000000000000000000000000000010101100001011000000010000000000000);
    return a;
}
