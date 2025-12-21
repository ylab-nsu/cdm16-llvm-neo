// CHECK reg(r0) 0

volatile int tick;

// correct
__attribute__((noinline))
void some_fun(){
  tick++;
}

// almost correct
__attribute__((noinline))
void some_fun_calling_fun(){
  some_fun();
}

// correct
__attribute__((noinline))
__attribute__((CDM_ISR))
void some_isr(){
  tick++;
}


__attribute__((noinline))
__attribute__((CDM_ISR))
void some_isr_calling_fun(){
  some_fun();
}

int main(){
  return 0;
}
