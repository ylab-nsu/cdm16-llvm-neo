// CHECK sym(value) AD DE

extern volatile int value;

int main() {
  int tmp;
  __asm__ __volatile__("ldi %[tmp], %[val]\n\t"
                       "stw %[ptr], %[tmp]\n\t"
                       : [ptr] "=m"(value), [tmp] "=&r"(tmp)
                       : [val] "i"(0xDEAD));
  return 0;
}
