// CHECK reg(r0) 0

#include "string.h"

int main(void) {
  char str[] = "string some string";
  char substr[] = "string";
  return (int) (strstr(str, substr) - str);
}
