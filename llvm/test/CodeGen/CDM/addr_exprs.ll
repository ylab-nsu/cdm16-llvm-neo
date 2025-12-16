target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for global variables

@string = global [6 x i8] c"Hello\00", align 1

; CHECK-LABEL: ptr1>
; CHECK: dc string+3
@ptr1 = global ptr getelementptr (i8, ptr @string, i64 3), align 2

; CHECK-LABEL: ptr2>
; CHECK: dc string-5
@ptr2 = global ptr getelementptr (i8, ptr @string, i64 -5), align 2
