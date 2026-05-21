// CHECK reg(r0) 0

#include "string.h"

int main(void) {
  char str[] = "";
  char substr[] = "s";
  return (int) strstr(str, substr);
}

