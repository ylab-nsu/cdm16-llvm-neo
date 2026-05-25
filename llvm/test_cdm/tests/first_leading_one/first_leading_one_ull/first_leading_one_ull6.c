// CHECK reg(r0) 36

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_ull((unsigned long long) 0b0000000000000000000000000000000000010000000000000000000000000001);
    return a;
}
