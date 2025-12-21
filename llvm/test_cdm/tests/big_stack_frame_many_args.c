// CHECK reg(r0) 3045

__attribute__((noinline)) 
int foo(int a, int b, int c, int d, int e, int f, int g) {
  volatile int lol[228] = {0};
  volatile int res = a ^ b;
  c &= d;
  g -= f;
  e += a - c + g;
  res |= e;
  return res;
}

int main() {
  return foo(69, 420, 52, 42, 1337, 228, 1488) - 6;
}
