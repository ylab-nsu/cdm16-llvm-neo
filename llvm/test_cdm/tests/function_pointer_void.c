// CHECK reg(r0) 0

typedef void (*volatile FuncT)(void);

__attribute__((noinline)) void storeVolatile() { *(int *volatile)0x8000 = 0x1; }

int main() {
  FuncT f = storeVolatile;
  f();

  return 0;
}
