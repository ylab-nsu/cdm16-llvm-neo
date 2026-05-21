// CHECK reg(r0) 0

typedef void (*volatile FuncT)(int *volatile);

__attribute__((noinline)) void storeVolatile(int *volatile Addr) {
  *(int *volatile)Addr = 0x1;
}

int main() {
  FuncT f = storeVolatile;

  f((int *volatile)0x8000);

  return 0;
}
