// CHECK reg(r0) 1
#include <string.h>

char a[] = "abOba";
char b[] = "aboba";

int main() {
    return strcmp(a, b) < 0 ? 1 : 0;
}
