// CHECK prod(end_to_end cocas)
// CHECK reg(r0) 233

__attribute__((noinline))
int fib(int n){
  if(n <= 1) return n;
  return fib(n - 1) + fib(n - 2);
}

int main(){
  return fib(13);
}


