// RUN: clang -target cdm -O0 -S -o /dev/stdout %s | FileCheck %s

// Regression test for storing long long on stack
// Bytes of i64 value used to be stored in wrong order because of incorrect alignment value

long long foo(long long value) {
// CHECK-LABEL: foo>
//
// CHECK-DAG: lsw [[RS1:r[0-6]]], -16
// CHECK-DAG: move r1, [[RS3:r[0-6]]]
// CHECK-DAG: lsw [[RS2:r[0-6]]], -14
// CHECK-DAG: move r0, [[RS4:r[0-6]]]
//
// CHECK-DAG: ldi [[RD1:r[0-6]]], -12
// CHECK-NEXT: add [[RD1]], fp, [[RD1]]
// CHECK-DAG: ldi [[RD21:r[0-6]]], 6
//
// CHECK: stw [[RD1]], [[RD21]], [[RS1]]
// CHECK-DAG: ldi [[RD22:r[0-6]]], 4
// CHECK: stw [[RD1]], [[RD22]], [[RS2]]
// CHECK-DAG: ldi [[RD23:r[0-6]]], 2
// CHECK: stw [[RD1]], [[RD23]], [[RS3]]
// CHECK: ssw [[RS4]], -12

    return value;
}
