// CHECK reg(r0) 11

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_ull((unsigned long long) 0b0000000000100000000000000000000000000000001000000000000000000000);
    return a;
}
