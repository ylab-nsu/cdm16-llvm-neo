// CHECK reg(r0) 0b0000000000000001
// CHECK reg(r1) 0b0000000000000000
// CHECK reg(r2) 0b0000000000000000
// CHECK reg(r3) 0b0000000000000000

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_ceil_ull((unsigned long long) 0b0000000000000000000000000000000000000000000000000000000000000000);
    return a;
}
