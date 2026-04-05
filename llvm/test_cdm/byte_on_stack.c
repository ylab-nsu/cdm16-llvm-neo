// CHECK reg(r0) 8801

__attribute__((noinline))
void store_byte_on_stack(char i, int j, char k, int l){
  volatile char a;
    volatile int b;
    volatile char c;
    volatile int d;
   a = i;
    b = j;
    c = k;
    d = l;
  // TODO: It leaves gaps
}

__attribute__((noinline))
int load_bytes_from_stack(){
  volatile char a = -95;
  volatile int b = 7331;
  volatile unsigned char c = 228;
  volatile int d = 1337;
  return a+b+c+d;
}

int main(){
  store_byte_on_stack('F', 1337, 'u', 7331);
  return load_bytes_from_stack();
}
