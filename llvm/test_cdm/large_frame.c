// CHECK reg(r0) 0

#ifdef __unix__
#include <stdio.h>
#endif

#define ANS 65024

unsigned SZ = 1024;

__attribute__((noinline)) unsigned short sum(volatile unsigned short *arr,
                                             unsigned sz) {
  unsigned short acc = 0;
  for (unsigned i = 0; i < sz; i++) {
    acc += arr[i];
  }

  return acc;
}

unsigned foo() {
  volatile char _frame[10000];
  volatile unsigned short arr[SZ];

  for (unsigned i = 0; i < SZ; i++)
    arr[i] = 65535 - i;

  unsigned short ans = sum(arr, SZ);

  return ans;
}

int main() {
  unsigned short ans = foo();

#ifdef __unix__
  printf("%hu\n", ans);
#endif

  return ans == ANS ? 0 : 1;
}
