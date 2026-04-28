// CHECK sym(dst) 48 45 6C 6C 6F

#include <string.h>

extern char dst[];

int main() {
    memccpy(dst, "HEllo", '1', 5);
    memccpy(dst, "GOOAL", 'A', 0);

    return 0;
}
