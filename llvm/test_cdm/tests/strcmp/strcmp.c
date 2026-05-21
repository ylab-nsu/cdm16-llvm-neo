// CHECK reg(r0) 0
#include <string.h>

char a[] = "aboba";
char b[] = "aboba";

int main() {
    return strcmp(a, b);
}
