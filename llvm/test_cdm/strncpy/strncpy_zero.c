// CHECK reg(r0) 0x48
// CHECK sym(dst) 48 45 6C 6C 6F 00

#include <string.h>

extern char dst[];

int main() {
    char *p = strncpy(dst, "HEllo", 6);
    p = strncpy(dst, "GOOAL", 0);

    return *p;
}
