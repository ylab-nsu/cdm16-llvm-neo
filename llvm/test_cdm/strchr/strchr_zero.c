// CHECK reg(r0) 6
#include <string.h>

char a[] = "aboboa";

int main() {
    return (int) ((char*) strchr(a, '\0') - a);
}
