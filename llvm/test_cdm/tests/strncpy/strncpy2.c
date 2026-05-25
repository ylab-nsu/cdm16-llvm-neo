// CHECK reg(r0) 1
// CHECK sym(dst) 48 45 6C 01 01 01

#include <string.h>

extern char dst[];

int main() {
    for (int i = 0; i < 6; i++) {
        dst[i] = 1;
    }
    return strncpy(dst, "HEllo", 3) == dst ? 1 : 0;
}
