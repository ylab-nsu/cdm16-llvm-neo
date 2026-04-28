// CHECK reg(r0) 1
#include <string.h>

char a[] = "abo";
char b[] = "aboba";

int main() {
    return strncmp(b, a, 6) > 0 ? 1 : 0;
}
