// CHECK reg(r0) 37

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_ull((unsigned long long) 0b1010111101101110101110101110000000000000000000000000000000000000);
    return a;
}
