// CHECK reg(r0) 5

#include <string.h>

const char a[] = "abcdef";

int main() {
    return strspn(a, "12decab14");
}

