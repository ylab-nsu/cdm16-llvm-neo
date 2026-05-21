// CHECK sym(dst) 48 45 6C 6C 6F
// CHECK reg(r0) 1

#include <string.h>

extern char dst[];

int main() {
    memmove(dst, "HEllo", 5);

    return memmove(dst, "GOOAL", 0) == dst ? 1 : 0;
}
