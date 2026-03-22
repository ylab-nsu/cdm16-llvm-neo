// CHECK prod(end_to_end)
// CHECK reg(r0) 0

#include "string.h"

int main(void) {
  return !(strlen("string with length 21") == 21);
}
