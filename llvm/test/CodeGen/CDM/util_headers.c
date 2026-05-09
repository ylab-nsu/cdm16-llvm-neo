// RUN: clang -target cdm-cocas -O0 -S -o - %s | FileCheck %s
// RUN: clang -target cdm-cocas -O1 -S -o - %s | FileCheck %s
// RUN: clang -target cdm-cocas -O2 -S -o - %s | FileCheck %s

// CDM-specific utility functions and macros

#include <cdm/interrupt.h>
#include <cdm/execution.h>
#include <cdm/ivt.h>
#include <cdm/register.h>

void test_halt(){
// CHECK-LABEL: test_halt>
// CHECK: halt
// CHECK-NOT: rts
  halt();
}

void test_reset(){
// CHECK-LABEL: test_reset>
// CHECK: reset
// CHECK-NOT: rts
  reset();
}

void test_wait(){
// CHECK-LABEL: test_wait>
// CHECK: wait
// CHECK: rts
  int_wait();
}

void test_ei(){
// CHECK-LABEL: test_ei>
// CHECK: ei
// CHECK: rts
  int_enable();
}

void test_di(){
// CHECK-LABEL: test_di>
// CHECK: di
// CHECK: rts
  int_disable();
}

void test_int(){
// CHECK-LABEL: test_int>
// CHECK: int 60
// CHECK: rts
  int_trigger(60);
}

int test_ldps(){
// CHECK-LABEL: test_ldps>
// CHECK: ldps {{r[0-6]}}
// CHECK: rts
  return ldps();
}

void test_stps(int value){
// CHECK-LABEL: test_stps>
// CHECK: stps {{r[0-6]}}
// CHECK: rts
  stps(value);
}

ISR void test_isr(){
// CHECK-LABEL: test_isr>
// CHECK: rti
// CHECK-NOT: rts
}
