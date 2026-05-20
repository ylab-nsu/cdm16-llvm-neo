// CHECK reg(r0) 28

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_ull((unsigned long long) 0b0000000000000000000000000001000000000000000000000000000000010000);
    return a;
}
