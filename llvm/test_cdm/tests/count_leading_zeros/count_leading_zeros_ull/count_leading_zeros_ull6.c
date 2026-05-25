// CHECK reg(r0) 27

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_ull((unsigned long long) 0b0000000000000000000000000001011000000101011110110111010111010110);
    return a;
}
