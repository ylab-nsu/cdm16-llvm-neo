// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_uc((unsigned char) 0b10000000);
    return a;
}
