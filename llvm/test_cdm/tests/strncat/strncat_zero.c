// CHECK reg(r0) 1
// CHECK sym(dst) 01 01 01 01 01 00

#include <string.h>

extern char dst[];

int main() {
    for (int i = 0; i < 6; i++) {
        dst[i] = 1;
    }
    dst[5] = '\0';
    char *p = strncat(dst, "HEllo", 0);

    return *p;
}
