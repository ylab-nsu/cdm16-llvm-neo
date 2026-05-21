// CHECK reg(r0) 6

__attribute__((noinline)) int foo(int a, int b, int c, int d, int e, int f,
                                  int g) {
  return f;
}

__attribute__((optnone)) int main() { return foo(1, 2, 3, 4, 5, 6, 7); }
