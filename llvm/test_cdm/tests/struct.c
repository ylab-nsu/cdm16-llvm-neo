// CHECK reg(r0) 0

typedef struct {
  int a;
  char b;
  char s[10];
} my_struct;


//volatile my_struct ss;
__attribute__((noinline))
int print(my_struct * s, int sz){
  volatile int a = s->a;
  return 0;
}


int main(){
  my_struct s = {
    1337, 'F', "sareg"
  };
  print(&s, sizeof(my_struct));
}



