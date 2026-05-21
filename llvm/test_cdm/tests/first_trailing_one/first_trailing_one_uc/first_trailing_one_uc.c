// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_uc((unsigned char) 0b00000001);
    return a;
}
