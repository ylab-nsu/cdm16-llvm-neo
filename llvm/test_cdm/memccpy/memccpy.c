// CHECK sym(dst) 48 45 6C 6C 6F
// CHECK reg(r0) 0

#include <string.h>

extern char dst[];

int main() {
    for (int i = 0; i < 5; i++) {
        dst[i] = '\0';
    }

    return (int) memccpy(dst, "HEllo", '1', 5);
}
