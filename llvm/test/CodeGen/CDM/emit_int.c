// RUN: clang -target cdm -O0 -S -o /dev/stdout %s | FileCheck %s

// Test emission of various integer constants in assembly

const long long int64 = 0xdeadbeefbebafac0;
// CHECK-LABEL: int64>
// CHECK: db 0xc0, 0xfa, 0xba, 0xbe, 0xef, 0xbe, 0xad, 0xde

const long int32 = 0x1102037f;
// CHECK-LABEL: int32>
// CHECK: db 0x7f, 0x03, 0x02, 0x11

const int int16 = 1337;
// CHECK-LABEL: int16>
// CHECK: dc 1337

const char int8 = 69;
// CHECK-LABEL: int8>
// CHECK: db 69
