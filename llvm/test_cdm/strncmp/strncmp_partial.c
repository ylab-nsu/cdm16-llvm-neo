// CHECK reg(r0) 0
#include <string.h>

char a[] = "abo";
char b[] = "aboba";

int main() {
    return strncmp(a, b, 3);
}

