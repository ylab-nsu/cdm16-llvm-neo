// CHECK reg(r0) 0

#include <string.h>

const char a[] = "abcdef";

int main() {
    return (int) strpbrk(a, "123242");
}


