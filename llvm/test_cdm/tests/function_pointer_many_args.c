// CHECK reg(r0) 69

typedef int (*volatile FuncT)(int, int, int, int, int);

__attribute__((noinline)) int foo(int x, int y, int z, int a, int b) {
  return x + y + z + (a << 1) + (b << 2);
}

int main() {
  FuncT f = foo;
  return f(4, 6, 7, 8, 9);
}
