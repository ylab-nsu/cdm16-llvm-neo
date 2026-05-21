// CHECK reg(r0) 11

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_ul((unsigned long) 0b00000000001000000000000000000000);
    return a;
}
