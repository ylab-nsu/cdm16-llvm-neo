// CHECK reg(r0) 4

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_uc((unsigned char) 0b00001000);
    return a;
}
