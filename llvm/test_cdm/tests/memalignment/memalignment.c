// CHECK reg(r0) 2

#include <stdlib.h>

int main() {
    void *p = (void*) 2;
    return (int) memalignment(p);
}
