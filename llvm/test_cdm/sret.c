// CHECK reg(r0) 14

struct S {
  long A;
  long long B;
  int C[10];
};

__attribute__((noinline))
struct S foo(int Start) {
  struct S Ret;
  Ret.A = Start++;
  Ret.B = Start++;
  for (int End = Start + 10; Start < End; Start++) {
    Ret.C[End - Start] = Start;
  }
  return Ret;
}

int main(void) {
  return foo(7).C[5];
}
