// CHECK sym(dst) 47 4F 4F 41 6F
// CHECK reg(r0) 4

#include <string.h>

extern char dst[];

int main() {
    memccpy(dst, "HEllo", '1', 5);

    return (int) ((char*) memccpy(dst, "GOOAL", 'A', 5) - dst);
}
