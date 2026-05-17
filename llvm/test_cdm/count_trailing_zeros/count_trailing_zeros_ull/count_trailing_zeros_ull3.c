// CHECK reg(r0) 64

#include <stdbit.h>

int main() {
    unsigned int a = stdc_trailing_zeros_ull((unsigned long long) 0);
    return a;
}
