// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_ul((unsigned long) 0b00000000000000000000000000000000);
    return a;
}

