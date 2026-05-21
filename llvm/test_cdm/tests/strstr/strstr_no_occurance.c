// CHECK reg(r0) 0

#include "string.h"

int main(void) {
  char str[] = "some strin some str";
  char substr[] = "string";
  return (int) strstr(str, substr);
}
