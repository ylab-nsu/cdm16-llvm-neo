// CHECK reg(r0) 2

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_uc((unsigned char) 0b11000000);
    return a;
}
