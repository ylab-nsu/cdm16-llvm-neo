target datalayout = "e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-n16-S16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

; Test for labels emitting for external symbols

declare i16 @foo(i16, i16)
; CHECK-LABEL: foo: ext

; We need to use the symbol so that the ext is emitted
define void @main() #0 {
    %result = call i16 @foo(i16 2, i16 4)
    ret void
}

