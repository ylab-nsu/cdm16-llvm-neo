// CHECK reg(r0) 0x48
// CHECK sym(dst) 48 45 6C 6C 6F 00 01

#include <string.h>

extern char dst[];

int main() {
    dst[6] = 1;
    char *p = strcpy(dst, "HEllo");

    return *p;
}
