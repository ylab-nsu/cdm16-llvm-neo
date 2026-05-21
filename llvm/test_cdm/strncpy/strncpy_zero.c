// CHECK reg(r0) 1
// CHECK sym(dst) 48 45 6C 6C 6F 00

#include <string.h>

extern char dst[];

int main() {
    strncpy(dst, "HEllo", 6);

    return strncpy(dst, "GOOAL", 0) == dst ? 1 : 0;
}
