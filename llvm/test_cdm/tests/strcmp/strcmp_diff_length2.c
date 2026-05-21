// CHECK reg(r0) 1
#include <string.h>

char a[] = "abo";
char b[] = "aboba";

int main() {
    return strcmp(b, a) > 0 ? 1 : 0;
}
