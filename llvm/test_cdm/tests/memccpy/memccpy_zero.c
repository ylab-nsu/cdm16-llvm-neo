// CHECK sym(dst) 48 45 6C 6C 6F
// CHECK reg(r0) 0

#include <string.h>

extern char dst[];

int main() {
    memccpy(dst, "HEllo", '1', 5);

    return (int) memccpy(dst, "GOOAL", 'A', 0);
}
