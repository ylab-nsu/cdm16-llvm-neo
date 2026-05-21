// CHECK sym(dst) 48 45 48 45 6C
// CHECK reg(r0) 1

#include <string.h>

extern char dst[];

int main() {
    dst[0] = 'H';
    dst[1] = 'E';
    dst[2] = 'l';
    dst[3] = 'l';
    dst[4] = 'o';

    return memmove(dst + 2, dst, 3) == dst + 2 ? 1 : 0;
}
