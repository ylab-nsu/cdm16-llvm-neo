// RUN: clang -target cdm-cocas -O1 -S -o - %s | FileCheck %s

#include <cdm/ivt.h>

extern void foo(context_t *ctx);
void bar(context_t *ctx) {
}

ISR void context_used(context_t *ctx) {
    // CHECK-LABEL: context_used>
    // CHECK: push fp
    // CHECK-NEXT: push r6
    // CHECK-NEXT: push r5
    // CHECK-NEXT: push r4
    // CHECK-NEXT: push r3
    // CHECK-NEXT: push r2
    // CHECK-NEXT: push r1
    // CHECK-NEXT: push r0
    // CHECK-NEXT: ldsp fp
    ctx->pc += 2;
    // CHECK-NEXT: lsw [[REG:r[0-9]]], 16
    // CHECK-NEXT: add [[REG]], 2
    // CHECK-NEXT: ssw [[REG]], 16
    // CHECK-NEXT: stsp fp
    // CHECK-NEXT: pop r0
    // CHECK-NEXT: pop r1
    // CHECK-NEXT: pop r2
    // CHECK-NEXT: pop r3
    // CHECK-NEXT: pop r4
    // CHECK-NEXT: pop r5
    // CHECK-NEXT: pop r6
    // CHECK-NEXT: pop fp
    // CHECK-NEXT: rti
}

ISR void context_used_indirectly(context_t *ctx) {
    // CHECK-LABEL: context_used_indirectly>
    // CHECK: push fp
    // CHECK-NEXT: push r6
    // CHECK-NEXT: push r5
    // CHECK-NEXT: push r4
    // CHECK-NEXT: push r3
    // CHECK-NEXT: push r2
    // CHECK-NEXT: push r1
    // CHECK-NEXT: push r0
    // CHECK-NEXT: ldsp fp
    foo(ctx);
    // CHECK: jsr foo
    // CHECK: stsp fp
    // CHECK-NEXT: pop r0
    // CHECK-NEXT: pop r1
    // CHECK-NEXT: pop r2
    // CHECK-NEXT: pop r3
    // CHECK-NEXT: pop r4
    // CHECK-NEXT: pop r5
    // CHECK-NEXT: pop r6
    // CHECK-NEXT: pop fp
    // CHECK-NEXT: rti
}

ISR void context_unused(context_t *ctx) {
    // CHECK-LABEL: context_unused>
    // CHECK-NOT: push r6
    // CHECK-NOT: push r5
    // CHECK-NOT: push r4
    context_t ctx2;
    foo(&ctx2);
    // CHECK: jsr foo
    // CHECK-NOT: pop r4
    // CHECK-NOT: pop r5
    // CHECK-NOT: pop r6
    // CHECK: rti
}

ISR void context_unused_optimized(context_t *ctx) {
    // CHECK-LABEL: context_unused_optimized>
    // CHECK-NOT: push r6
    // CHECK-NOT: push r5
    // CHECK-NOT: push r4
    // CHECK-NOT: push r3
    // CHECK-NOT: push r2
    // CHECK-NOT: push r1
    // CHECK-NOT: push r0
    bar(ctx);
    // CHECK-NOT: pop r0
    // CHECK-NOT: pop r1
    // CHECK-NOT: pop r2
    // CHECK-NOT: pop r3
    // CHECK-NOT: pop r4
    // CHECK-NOT: pop r5
    // CHECK-NOT: pop r6
    // CHECK: rti
}
