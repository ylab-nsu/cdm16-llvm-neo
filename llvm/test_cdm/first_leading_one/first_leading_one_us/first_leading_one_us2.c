// CHECK reg(r0) 11

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_us((unsigned short) 0b0000000000100000);
    return a;
}

