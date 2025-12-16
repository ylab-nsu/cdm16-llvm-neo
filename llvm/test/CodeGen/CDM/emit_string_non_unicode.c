// RUN: clang -target cdm -O0 -S -o /dev/stdout %s | FileCheck %s

// cocas strings are unicode-only so we emit
// strings with non-ascii characters as byte arrays
// because we can't directly insert bytes above 0x7f

const char string1[] = "\x7f\x70\005a";
// CHECK-LABEL: string1>
// CHECK: db "\177p\005a\000"

const char string2[] = "\x7f\x80\005a";
// CHECK-LABEL: string2>
// CHECK: db 0x7f, 0x80, 0x05, 0x61, 0x00

const char string3[] = "ого!";
// CHECK-LABEL: string3>
// CHECK: db 0xd0, 0xbe, 0xd0, 0xb3, 0xd0, 0xbe, 0x21, 0x00
