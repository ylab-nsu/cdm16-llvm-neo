target datalayout = "e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-n16-S16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

; Test for global variables

; CHECK-LABEL: a>
; CHECK: dc 5
@a = global i16 5, align 2
