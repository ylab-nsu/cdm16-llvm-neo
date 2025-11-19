target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for emitting ext labels for multiple externalsymbols

declare i16 @foo(i16, i16)
; CHECK-LABEL: foo: ext
declare i16 @foo2(i16, i16)
; CHECK-LABEL: foo2: ext
declare i16 @foo3(i16, i16)
; CHECK-LABEL: foo3: ext
declare i16 @foo4(i16, i16)
; CHECK-LABEL: foo4: ext
