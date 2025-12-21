// CHECK reg(r0) 6

__attribute__((noinline)) int foo(int a, int b, int c, int d, int e, int f,
                                  int g) {
  return f;
}

__attribute__((optnone)) int main() {
  int a = 4;
  int b = 5;
  int c = foo(a, b, 3, 4, 5, 67, 7);
  int d = foo(1, 2, b, a, 67, 6, c);
  int f = foo(1, 2, c, b, 5, 16, a);
  return d;
}
