// CHECK reg(r0) 6

__attribute__((noinline)) int foo(char a, float b, long long c, long d, short e,
                                  int f, double g) {
  return f;
}

__attribute__((optnone)) int main() { return foo(1, 2, 3, 4, 5, 6, 7); }
