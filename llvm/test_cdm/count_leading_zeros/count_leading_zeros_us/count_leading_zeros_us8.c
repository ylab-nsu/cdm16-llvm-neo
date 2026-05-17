// CHECK reg(r0) 16

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0);
    return a;
}
