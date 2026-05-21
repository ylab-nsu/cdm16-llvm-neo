// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    return stdc_has_single_bit_ull((unsigned long long) 0b0000000000000100000000000000000000000000000000000000000000000000);
}
