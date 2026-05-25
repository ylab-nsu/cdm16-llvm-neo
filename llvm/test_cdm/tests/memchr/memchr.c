// CHECK reg(r0) 2
#include <string.h>

char a[] = "aboboa";

int main() {
    return (int) ((char*) memchr(a, 'o', sizeof(a) - 1) - a);
}
