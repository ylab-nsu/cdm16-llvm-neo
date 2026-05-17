// CHECK reg(r0) 5

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_ull((unsigned long long) 0b0000010101111011011101011101011000000101011110110111010111010110);
    return a;
}
