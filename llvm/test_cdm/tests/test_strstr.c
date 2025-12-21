// CHECK reg(r0) 0

#include "string.h"

int main(void) {
  char str[] = "some string some string";
  char substr[] = "string";
  return strstr(str, substr) - (str + 5);
}
