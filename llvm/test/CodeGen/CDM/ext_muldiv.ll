target datalayout = "e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-n16-S16"

; RUN: llc -mtriple=cdm-cocas -mattr=+m < %s | FileCheck %s

; Lowering of trap intrinsics

define i16 @mul(i16 %a, i16 %b) {
; CHECK-LABEL: mul>
; CHECK: smul {{r[0-6]}}, {{r[0-6]}}
    %result = mul i16 %a, %b
    ret i16 %result
}

define i16 @sdiv(i16 %a, i16 %b) {
; CHECK-LABEL: sdiv>
; CHECK: sdiv {{r[0-6]}}, {{r[0-6]}}
    %result = sdiv i16 %a, %b
    ret i16 %result
}

define i16 @udiv(i16 %a, i16 %b) {
; CHECK-LABEL: udiv>
; CHECK: udiv {{r[0-6]}}, {{r[0-6]}}
    %result = udiv i16 %a, %b
    ret i16 %result
}

define i16 @srem(i16 %a, i16 %b) {
; CHECK-LABEL: srem>
; CHECK: sdiv {{r[0-6]}}, {{r[0-6]}}
    %result = srem i16 %a, %b
    ret i16 %result
}

define i16 @urem(i16 %a, i16 %b) {
; CHECK-LABEL: urem>
; CHECK: udiv {{r[0-6]}}, {{r[0-6]}}
    %result = urem i16 %a, %b
    ret i16 %result
}
