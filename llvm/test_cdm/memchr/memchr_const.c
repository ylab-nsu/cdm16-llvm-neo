// CHECK reg(r0) 2
#include <string.h>

const char a[] = "aboboa";

int main() {
    const char *c = memchr(a, 'o', sizeof(a) - 1);
    return (int) (c - a);
}
