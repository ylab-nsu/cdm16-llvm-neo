// CHECK reg(r0) 14

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_us((unsigned short) 0b00000010);
    return a;
}
