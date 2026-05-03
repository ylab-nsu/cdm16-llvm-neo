// CHECK reg(r0) 0

#include <string.h>

const char a[] = "";

int main() {
    return (int) strpbrk(a, "abcdf");
}

