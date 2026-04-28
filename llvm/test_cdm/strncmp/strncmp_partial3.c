// CHECK reg(r0) 1
#include <string.h>

char a[] = "aBo";
char b[] = "aboba";

int main() {
    return strncmp(b, a, 3) > 0 ? 1 : 0;
}

