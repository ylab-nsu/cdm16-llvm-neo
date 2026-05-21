// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_us((unsigned short) 0b1000000000000000);
    return a;
}
