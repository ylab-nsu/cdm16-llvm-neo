// CHECK reg(r0) 11

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_us((unsigned short) 0b0000010000000000);
    return a;
}

