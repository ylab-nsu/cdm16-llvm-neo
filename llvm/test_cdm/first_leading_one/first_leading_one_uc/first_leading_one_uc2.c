// CHECK reg(r0) 5

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_uc((unsigned char) 0b00001000);
    return a;
}
