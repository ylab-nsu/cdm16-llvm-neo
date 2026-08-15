// RUN: clang -target cdm-cocas -O0 -S -o - %s | FileCheck %s

// Regression test for storing long long on stack
// Bytes of i64 value used to be stored in wrong order because of incorrect alignment value

long long foo(long long value) {
// CHECK-LABEL: foo>
//
// CHECK-DAG: ssw r0, -[[L0:[0-9]+]]
// CHECK-DAG: ssw r1, -[[L1:[0-9]+]]
// CHECK-DAG: ssw r2, -[[L2:[0-9]+]]
// CHECK-DAG: ssw r3, -[[L3:[0-9]+]]
// CHECK: {{.*}}
// CHECK-DAG: lsw r0, -[[L0]]
// CHECK-DAG: lsw r1, -[[L1]]
// CHECK-DAG: lsw r2, -[[L2]]
// CHECK-DAG: lsw r3, -[[L3]]

    return value;
}
