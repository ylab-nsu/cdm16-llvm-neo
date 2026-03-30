// CHECK prod(end_to_end)
// CHECK sym(dst) 48 45 6C 6C 6F
// CHECK prod(driver)

extern char dst[];

__attribute__((noinline))
void flip_case (char* c){
  *c ^= ' ';
}

__attribute__((noinline))
void add_ten(int *i){
  *i += 10;
}

__attribute__((noinline))
void write_str_to_mem(char *s){
  for (int i = 0; *s != '\0'; i++){
      dst[i] = *(s++);
  }
}


int main(){
  char str[] = "Hello";
  flip_case(str + 1);
  write_str_to_mem(str);
}
