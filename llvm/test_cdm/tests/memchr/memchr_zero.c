// CHECK reg(r0) 6
#include <string.h>

char a[] = "aboboa";

int main() {
    return (int) ((char*) memchr(a, '\0', sizeof(a)) - a);
}
