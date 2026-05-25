// CHECK sym(dst) 6C 6C 6F 6C 6F
// CHECK reg(r0) 1

#include <string.h>

extern char dst[];

int main() {
    dst[0] = 'H';
    dst[1] = 'E';
    dst[2] = 'l';
    dst[3] = 'l';
    dst[4] = 'o';

    return memmove(dst, dst + 2, 3) == dst ? 1 : 0;
}
