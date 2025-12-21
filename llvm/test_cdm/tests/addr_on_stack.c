// CHECK mem(0xFF00) 48 45 6C 6C 6F

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
  char* dst = (char*) 0xFF00;
  while (*s)*(dst++) = *(s++);
}


int main(){
  char str[] = "Hello";
  flip_case(str + 1);
  write_str_to_mem(str);
}
