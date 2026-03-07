// RUN: clang -target cdm-cocas -O1 -S -o /dev/stdout %s | FileCheck %s

// CDM-specific clang builtins mapped to LLVM intrinsics and then instructions

void halt(){
// CHECK-LABEL: halt>
// CHECK: halt
  __builtin_cdm_halt();
}

void wait(){
// CHECK-LABEL: wait>
// CHECK: wait
  __builtin_cdm_wait();
}

void ei(){
// CHECK-LABEL: ei>
// CHECK: ei
  __builtin_cdm_ei();
}

void di(){
// CHECK-LABEL: di>
// CHECK: di
  __builtin_cdm_di();
}

void interrupt(){
// CHECK-LABEL: interrupt>
// CHECK: int 69
  __builtin_cdm_int(69);
}

void reset(){
// CHECK-LABEL: reset>
// CHECK: reset 420
  __builtin_cdm_reset(420);
}

int ldps(){
// CHECK-LABEL: ldps>
// CHECK: ldps {{r[0-6]}}
  return __builtin_cdm_ldps();
}

void stps(int value){
// CHECK-LABEL: stps>
// CHECK: stps {{r[0-6]}}
  __builtin_cdm_stps(value);
}
