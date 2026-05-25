// CHECK reg(r0) 1
// CHECK sym(dst) 48 45 6C 00 00 00

#include <string.h>

extern char dst[];

int main() {
    for (int i = 0; i < 6; i++) {
        dst[i] = 1;
    }

    return strncpy(dst, "HEl", 6) == dst ? 1 : 0;
}
