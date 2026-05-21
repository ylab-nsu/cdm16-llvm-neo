// CHECK reg(r0) 32

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_ul((unsigned long) 0b10000000000000000000000000000000);
    return a;
}
