// CHECK reg(r0) 17

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_ul((unsigned long) 0b00000000000000000111010111010110);
    return a;
}
