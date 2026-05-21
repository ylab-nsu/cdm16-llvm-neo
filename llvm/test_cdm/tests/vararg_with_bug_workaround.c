// CHECK reg(r0) 36

#include <stdarg.h>

// On early stage of vararg development I need to pass at least 4 arguments to
// the variadic function to make it work.
__attribute__((noinline)) int sum(int Count, int A, int B, int C, ...) {
  va_list Args;
  va_start(Args, C);

  int Sum = 0;
  for (int I = 0; I < Count; I++) {
    Sum += va_arg(Args, int);
  }

  va_end(Args);
  return Sum;
}

int main(void) { return sum(8, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8); }
