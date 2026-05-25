// CHECK reg(r0) 46

struct S {
  char A;
  int B;
  long C;
  long long D;
};

__attribute__((noinline))
int processStruct(struct S A) {
  A.D = A.A;
  A.C = A.B;
  return A.D + A.C;
}

int main(void) {
  struct S A = {.A = 1, .B = 2, .C = 21, .D = 22};
  int Res = processStruct(A);
  return Res + A.C + A.D;
}
