// RUN: clang -target cdm -O0 -S -emit-llvm -o /dev/stdout %s | llc -mtriple=cdm | FileCheck %s

long long foo(long long value) {
// CHECK-LABEL: foo>
// CHECK: ldi r4, -12
// CHECK-NEXT: add r4, fp, r4
// CHECK-NEXT: ldi r5, 6
// CHECK-NEXT: stw r4, r5, r3
// CHECK-NEXT: ldi r3, 4
// CHECK-NEXT: stw r4, r3, r2
// CHECK-NEXT: ldi r2, 2
// CHECK-NEXT: stw r4, r2, r1
// CHECK-NEXT: ssw r0, -12
    return value;
}
