// CHECK sym(dst) 47 4F 4F 41 6F

#include <string.h>

extern char dst[];

int main() {
    memccpy(dst, "HEllo", '1', 5);

    memccpy(dst, "GOOAL", 'A', 5);

    return 0;
}
