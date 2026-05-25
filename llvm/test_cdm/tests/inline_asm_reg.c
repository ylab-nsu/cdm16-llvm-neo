// CHECK sym(new_value) AD DE
// CHECK sym(old_value) EF BE

extern volatile int new_value;
extern volatile int old_value;

static int exchange(volatile int *ptr, int value) {
  int result;
  int sreg;
  __asm__ __volatile__("stps %[tmp]\n\t"
                       "di\n\t"
                       "ldw %[ptr], %[res]\n\t"
                       "stw %[ptr], %[val]\n\t"
                       "ldps %[tmp]\n\t"
                       : [res] "=&r"(result), [tmp] "=&r"(sreg)
                       : [ptr] "r"(ptr), [val] "r"(value));
  return result;
}

int main() {
  new_value = 0xBEEF;
  old_value = exchange(&new_value, 0xDEAD);
  return 0;
}
