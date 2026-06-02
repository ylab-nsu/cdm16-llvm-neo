// RUN: clang -target cdm-cocas -O0 -S -o - %s | FileCheck %s

__progmem extern const int progmem_constant;

int foo() {
// CHECK-LABEL: foo>
// CHECK: lcw {{r[0-6]}}, {{r[0-6]}}
    return progmem_constant;
}

signed int load_ptr(__progmem const signed char *ptr) {
// CHECK-LABEL: load_ptr>
// CHECK: lcsb {{r[0-6]}}, {{r[0-6]}}
    return *ptr;
}

signed int load_ptr_cast(void *ptr) {
// CHECK-LABEL: load_ptr_cast>
// CHECK: lcb {{r[0-6]}}, {{r[0-6]}}
    return *(__progmem const unsigned char*)ptr;
}

// CHECK-LABEL: progmem_constant: ext
