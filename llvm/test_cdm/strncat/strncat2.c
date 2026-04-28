// CHECK reg(r0) 0x48
// CHECK sym(dst) 48 45 6C 00 01 01

#include <string.h>

extern char dst[];

int main() {
    dst[0] = '\0';
    for (int i = 1; i < 6; i++) {
        dst[i] = 1;
    }
    char *p = strncat(dst, "HEl", 3);

    return *p;
}
