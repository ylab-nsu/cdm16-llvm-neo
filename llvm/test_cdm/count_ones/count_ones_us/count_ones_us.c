// CHECK reg(r0) 0

#include <stdbit.h>

int main() {
    unsigned int a = stdc_count_ones_us((unsigned short) 0b0000000000000000);
    return a;
}
