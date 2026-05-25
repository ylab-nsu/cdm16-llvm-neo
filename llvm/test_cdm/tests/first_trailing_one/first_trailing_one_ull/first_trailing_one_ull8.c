// CHECK reg(r0) 53

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_ull((unsigned long long) 0b0000000000110000000000000000000000000000000000000000000000000000);
    return a;
}
