// CHECK reg(r0) 10

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b00110101);
    return a;
}
