// CHECK reg(r0) 37

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_ull((unsigned long long) 0b00000101011110110111010111010110);
    return a;
}
