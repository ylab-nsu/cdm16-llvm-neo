// CHECK reg(r0) 6

#include <string.h>

const char a[] = "abcdef";

int main() {
    return strspn(a, "fedcab");
}


