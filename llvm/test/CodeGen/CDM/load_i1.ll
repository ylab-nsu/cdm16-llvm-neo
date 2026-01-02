target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test i1 loads (should be promoted to i8)

@foo = global i1 false

define i1 @get_foo() {
; CHECK-LABEL: get_foo>
; CHECK: ldi [[REG:r[0-6]]], foo
; CHECK-NEXT: ldb [[REG]], {{r[0-6]}}
    %result = load i1, ptr @foo
    ret i1 %result
}
