target datalayout = "e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-n16-S16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

define i32 @add32(i32 %a, i32 %b) #0 {
; CHECK-LABEL: add32>
; CHECK: add {{r[0-6]}}, {{r[0-6]}}, {{r[0-6]}}
; CHECK-NEXT: addc {{r[0-6]}}, {{r[0-6]}}, {{r[0-6]}}
    %result = add i32 %a, %b
    ret i32 %result
}

define i32 @sub32(i32 %a, i32 %b) #0 {
; CHECK-LABEL: sub32>
; CHECK: sub {{r[0-6]}}, {{r[0-6]}}, {{r[0-6]}}
; CHECK-NEXT: subc {{r[0-6]}}, {{r[0-6]}}, {{r[0-6]}}
    %result = sub i32 %a, %b
    ret i32 %result
}

define i32 @neg32(i32 %a) #0 {
; CHECK-LABEL: neg32>
; CHECK: sub {{r[0-6]}}, {{r[0-6]}}, {{r[0-6]}}
; CHECK-NEXT: subc {{r[0-6]}}, {{r[0-6]}}, {{r[0-6]}}
    %result = sub i32 0, %a
    ret i32 %result
}

define i64 @add64(i64 %a, i64 %b) #0 {
; CHECK-LABEL: add64>
; CHECK: jsr __adddi3
    %result = add i64 %a, %b
    ret i64 %result
}

define i64 @sub64(i64 %a, i64 %b) #0 {
; CHECK-LABEL: sub64>
; CHECK: jsr __subdi3
    %result = sub i64 %a, %b
    ret i64 %result
}

define i64 @neg64(i64 %a) #0 {
; CHECK-LABEL: neg64>
; CHECK: jsr __negdi2
    %result = sub i64 0, %a
    ret i64 %result
}

; CHECK-LABEL: __adddi3: ext
; CHECK-LABEL: __subdi3: ext
; CHECK-LABEL: __negdi2: ext

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
