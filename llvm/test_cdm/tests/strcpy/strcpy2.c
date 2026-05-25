// CHECK reg(r0) 1
// CHECK sym(dst) 48 45 6C 6C 6F 00 01

#include <string.h>

extern char dst[];

int main() {
    dst[6] = 1;

    return strcpy(dst, "HEllo") == dst ? 1 : 0;
}
