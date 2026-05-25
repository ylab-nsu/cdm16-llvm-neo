// CHECK reg(r0) 32

#include <stdbit.h>

int main() {
    unsigned int a = stdc_leading_zeros_ul((unsigned long) 0);
    return a;
}
