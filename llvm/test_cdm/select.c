// CHECK reg(r0) 228

__attribute__((noinline))
int f1(int a){
  return a > 15 ? 1337 : 228;
}

__attribute__((noinline))
int f2(int a, int b){
  return a < b;
}

__attribute__((noinline))
int f3(_Bool c){
  return c ? 1447 : 228;
}


int main(){
  volatile int a = f1(16);
  return f1(3);
}
