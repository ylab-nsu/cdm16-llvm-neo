// CHECK reg(r0) 16

#include "string.h"

int main(void) {
  char str[] = "some strin some string";
  char substr[] = "string";
  return (int) (strstr(str, substr) - str);
}
