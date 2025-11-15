// RUN: clang -target cdm -O1 -S -emit-llvm -o /dev/stdout %s | llc -mtriple=cdm | FileCheck %s

void foo(char a, char b, char c){
// CHECK-LABEL: foo>
// CHECK-NEXT: # %bb.0:
// CHECK-NEXT: push fp
// CHECK-NEXT: ldsp fp
// CHECK-NEXT: addsp -4
// CHECK-NEXT: ssb r0, -1
// CHECK-NEXT: ssb r1, -2
// CHECK-NEXT: ssb r2, -3
// CHECK-NEXT: addsp 4
// CHECK-NEXT: stsp fp
// CHECK-NEXT: pop fp
// CHECK-NEXT: rts
	volatile char arr[] = {a, b, c};
}
