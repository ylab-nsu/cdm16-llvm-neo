// CHECK sym(dst) 02 02 02 02 02

#include <string.h>

extern char dst[];

int main() {
    for (int i = 0; i < 5; i++) {
        dst[i] = 1;
    }
    memset(dst, 2, 5);

    return 0;
}
