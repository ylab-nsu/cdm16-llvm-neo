// CHECK reg(r0) 62

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_ull((unsigned long long) 0b0110000000000000000000000000000000000000000000000000000000000000);
    return a;
}

