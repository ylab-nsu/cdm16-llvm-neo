// RUN: clang -target cdm -O1 -S -o /dev/stdout %s | FileCheck %s

void foo(char a, char b, char c){
// CHECK-LABEL: foo>
// CHECK: addsp -4
// CHECK-NEXT: ssb r0, -1
// CHECK-NEXT: ssb r1, -2
// CHECK-NEXT: ssb r2, -3
// CHECK-NEXT: addsp 4
	volatile char arr[] = {a, b, c};
}
