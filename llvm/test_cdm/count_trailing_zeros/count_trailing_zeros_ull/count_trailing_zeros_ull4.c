// CHECK reg(r0) 5

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_ull((unsigned long long) 0b1010111101101110101110101100000010101111011011101011101011100000);
    return a;
}
