// CHECK reg(r0) 2

#include <string.h>

const char a[] = "abcdef";

int main() {
    return strspn(a, "baef");
}

