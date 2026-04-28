// CHECK sym(dst) 48 45 6C 6C 6F

#include <string.h>

extern char dst[];

int main() {
    memcpy(dst, "HEllo", 5);

    return 0;
}
