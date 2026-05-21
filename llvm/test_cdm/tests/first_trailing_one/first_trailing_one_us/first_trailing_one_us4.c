// CHECK reg(r0) 16

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_us((unsigned short) 0b1000000000000000);
    return a;
}

