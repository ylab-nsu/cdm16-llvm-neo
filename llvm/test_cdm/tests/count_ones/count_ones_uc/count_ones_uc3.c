// CHECK reg(r0) 4

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_uc((unsigned char) 0b10011001);
    return a;
}
