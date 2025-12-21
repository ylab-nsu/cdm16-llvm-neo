// CHECK reg(r0) 36

#include <stdarg.h>

__attribute__((noinline)) int sum(int Count, ...) {
  va_list Args;
  va_start(Args, Count);

  int Sum = 0;
  for (int I = 0; I < Count; I++) {
    Sum += va_arg(Args, int);
  }

  va_end(Args);
  return Sum;
}

int main(void) { return sum(8, 1, 2, 3, 4, 5, 6, 7, 8); }
