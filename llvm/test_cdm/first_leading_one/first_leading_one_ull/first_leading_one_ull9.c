// CHECK reg(r0) 62

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_ull((unsigned long long) 0b0000000000000000000000000000000000000000000000000000000000000101);
    return a;
}

