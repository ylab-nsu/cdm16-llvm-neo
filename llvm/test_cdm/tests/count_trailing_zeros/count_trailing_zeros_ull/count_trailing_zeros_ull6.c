// CHECK reg(r0) 27

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_ull((unsigned long long) 0b1011000000101011110110111010111010111000000000000000000000000000);
    return a;
}
