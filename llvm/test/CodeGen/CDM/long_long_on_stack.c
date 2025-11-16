// RUN: clang -target cdm -O1 -S -emit-llvm -o /dev/stdout %s | llc -mtriple=cdm | FileCheck %s

__attribute__((noinline))
long long foo(long long a, long long b){
	return a + b;
}

long long bar(){
// CHECK-LABEL: bar>
// CHECK-NEXT: # %bb.0:
// CHECK-NEXT: push fp
// CHECK-NEXT: ldsp fp
// CHECK-NEXT: addsp -16
// CHECK-NEXT: ldsp r0
// CHECK-NEXT: ldi r1, 10
// CHECK-NEXT: ldi r2, 1883
// CHECK-NEXT: stw r0, r1, r2
// CHECK-NEXT: ldi r1, 8
// CHECK-NEXT: ldi r2, -13035
// CHECK-NEXT: stw r0, r1, r2
// CHECK-NEXT: ldi r2, 14
// CHECK-NEXT: ldi r1, 0
// CHECK-NEXT: stw r0, r2, r1
// CHECK-NEXT: ldi r2, 12
// CHECK-NEXT: stw r0, r2, r1
// CHECK-NEXT: ldi r0, 2
// CHECK-NEXT: move r1, r2
// CHECK-NEXT: move r1, r3
// CHECK-NEXT: jsr foo
// CHECK-NEXT: addsp 16
// CHECK-NEXT: stsp fp
// CHECK-NEXT: pop fp
// CHECK-NEXT: rts
	return foo(2, 123456789);
}
