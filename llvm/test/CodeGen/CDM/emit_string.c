// RUN: clang -target cdm -O0 -S -o /dev/stdout %s | FileCheck %s

// Test emission of strings in assembly

const char string1[] = "Hello, world!";
// CHECK-LABEL: string1>
// CHECK: db "Hello, world!\000"

const char string2[] = { 'H', 'i', 'a' };
// CHECK-LABEL: string2>
// CHECK: db "Hia"
