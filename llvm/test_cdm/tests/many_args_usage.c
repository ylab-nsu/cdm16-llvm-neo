// CHECK reg(r0) 69

__attribute__((noinline)) int foo(int x, int y, int z, int a, int b) {
  return x + y + z + (a << 1) + (b << 2);
}

__attribute__((optnone)) int main() { return foo(4, 6, 7, 8, 9); }
