// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_bit_floor_ull((unsigned long long) 0b0000000000000000000000000000000000000000000000000000000000000000);
    return a;
}
