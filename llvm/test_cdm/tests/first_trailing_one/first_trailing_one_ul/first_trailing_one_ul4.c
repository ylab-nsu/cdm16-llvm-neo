// CHECK reg(r0) 28

#include <stdbit.h>

int main() {
    unsigned int a = stdc_first_trailing_one_ul((unsigned long) 0b00001000000000000000000000000000);
    return a;
}
