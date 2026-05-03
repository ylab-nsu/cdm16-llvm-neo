// CHECK reg(r0) 4
#include <string.h>

char a[] = "aboboa";

int main() {
    return (int) ((char*) strrchr(a, 'o') - a);
}
