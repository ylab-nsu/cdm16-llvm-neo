// CHECK reg(r0) 0
#include <string.h>

char a[] = "aboba";

int main() {
    return (int) strchr(a, 'c');
}

