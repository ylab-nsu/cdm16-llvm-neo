// CHECK reg(r0) 2
#include <string.h>

const char a[] = "aboboa";

int main() {
    const char *c = strchr(a, 'o');
    return (int) (c - a);
}
