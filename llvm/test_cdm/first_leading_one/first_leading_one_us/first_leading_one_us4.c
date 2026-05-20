// CHECK reg(r0) 16

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_us((unsigned short) 0b0000000000000001);
    return a;
}

