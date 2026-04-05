// CHECK reg(r0) 1342
// CHECK mem(0xCAFE) BE BA

int a = 5;
int b;

__attribute__((noinline))
void add(int c){
  b = a + c;
}

int main(){
  add(1337);
  *((int *)(0xCAFE)) = 0xBABE;
  return b;
}
