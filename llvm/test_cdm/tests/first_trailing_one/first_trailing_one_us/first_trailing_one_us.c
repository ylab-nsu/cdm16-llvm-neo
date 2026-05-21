// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_us((unsigned short) 0b0000000000000001);
    return a;
}
