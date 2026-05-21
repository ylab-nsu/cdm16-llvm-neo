// CHECK reg(r0) 1
#include <string.h>

char a[] = "aBo";
char b[] = "aboba";

int main() {
    return strncmp(a, b, 3) < 0 ? 1 : 0;
}

