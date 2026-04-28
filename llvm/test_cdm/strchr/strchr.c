// CHECK reg(r0) 2
#include <string.h>

char a[] = "aboboa";

int main() {
    return (int) ((char*) strchr(a, 'o') - a);
}
