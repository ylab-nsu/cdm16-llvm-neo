// CHECK reg(r0) 0x48
// CHECK sym(dst) 48 45 6C 00 00 00

#include <string.h>

extern char dst[];

int main() {
    for (int i = 0; i < 6; i++) {
        dst[i] = 1;
    }

    char *p = strncpy(dst, "HEl", 6);

    return *p;
}
