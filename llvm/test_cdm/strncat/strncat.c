// CHECK reg(r0) 0x48
// CHECK sym(dst) 48 45 6C 6C 6F 00

#include <string.h>

extern char dst[];

int main() {
    dst[0] = '\0';
    char *p = strncat(dst, "HEl", 3);
    p = strncat(dst, "lo", 2);

    return *p;
}
