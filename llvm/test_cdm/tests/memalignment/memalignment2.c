// CHECK reg(r0) 16

#include <stdlib.h>

int main() {
    void *p = (void*) 48;
    return (int) memalignment(p);
}
