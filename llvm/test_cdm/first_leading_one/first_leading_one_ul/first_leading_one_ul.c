// CHECK reg(r0) 1

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_leading_one_ul((unsigned long) 0b10000000000000000000000000000000);
    return a;
}
