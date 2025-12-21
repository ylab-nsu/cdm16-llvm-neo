// CHECK reg(r0) 30

#include <string.h>

struct S {
  int A[10];
};

__attribute__((noinline))
int sum(struct S V) {
  int Sum = 0;
  for (int I = 0; I < 10; I++) {
    Sum += ++V.A[I];
  }
  return Sum;
}

int main(void) {
  struct S V = {.A = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
  int Sum = sum(V);
  for (int I = 0; I < 10; I++) {
    Sum += V.A[I];
  }
  return Sum;
}
