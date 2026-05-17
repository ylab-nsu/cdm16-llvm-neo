// CHECK reg(r0) 0

#include <stdlib.h>

int main() {
    void *p = NULL;
    return (int) memalignment(p);
}

