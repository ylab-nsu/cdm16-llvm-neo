// CHECK reg(r0) 8

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_uc((unsigned char) 0b10000000);
    return a;
}
