// RUN: clang -target cdm-cocas -O0 -S -o - %s | FileCheck %s

// non-ASCII characters should be encoded as octal

const char string1[] = "\x7f\x70\005a";
// CHECK-LABEL: string1>
// CHECK: db "\177p\005a\000"

const char string2[] = "\x7f\x80\005a";
// CHECK-LABEL: string2>
// CHECK: db "\177\200\005a\000"

const char string3[] = "ого!";
// CHECK-LABEL: string3>
// CHECK: db "\320\276\320\263\320\276!\000"
