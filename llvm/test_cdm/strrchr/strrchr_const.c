// CHECK reg(r0) 4
#include <string.h>

const char a[] = "aboboa";

int main() {
    const char *c = strrchr(a, 'o');
    return (int) (c - a);
}
