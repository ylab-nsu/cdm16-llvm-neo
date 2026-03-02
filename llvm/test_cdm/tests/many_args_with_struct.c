// CHECK prod(end_to_end cocas)
// CHECK reg(r0) 35
// CHECK prod(driver_only)

struct S {
  int A[10];
};

__attribute__((noinline)) int foo(char A, struct S B, int C, long long E) {
  return A + B.A[4] + C + E;
}

__attribute__((optnone)) int main(void) {
  return foo(1, (struct S){{2, 3, 4, 5, 6, 7, 8, 9, 10, 11}}, 13, 15ll);
}
