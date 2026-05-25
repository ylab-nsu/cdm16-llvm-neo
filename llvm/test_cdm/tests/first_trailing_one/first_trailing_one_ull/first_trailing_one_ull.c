// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_ull((unsigned long long) 0b0000000000000000000000000000000100000000000000000000000000000001);
    return a;
}
