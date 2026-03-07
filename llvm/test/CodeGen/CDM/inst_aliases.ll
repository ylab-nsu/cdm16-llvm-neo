target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

; Instruction alias emission tests

define i16 @inc(i16 %a) #0 {
; CHECK-LABEL: inc>
; CHECK: inc {{r[0-6]}}
    %result = add i16 %a, 1
    ret i16 %result
}

define i16 @dec(i16 %a) #0 {
; CHECK-LABEL: dec>
; CHECK: dec {{r[0-6]}}
    %result = add i16 %a, -1
    ret i16 %result
}

define i16 @tst(i16 %a) #0 {
; CHECK-LABEL: tst>
; CHECK: tst {{r[0-6]}}
    %cmp = icmp eq i16 %a, 0
    %result = select i1 %cmp, i16 69, i16 420
    ret i16 %result
}

; Should not use aliases
define i16 @add2(i16 %a) #0 {
; CHECK-LABEL: add2>
; CHECK: add {{r[0-6]}}, 2
    %result = add i16 %a, 2
    ret i16 %result
}

; Should not use aliases
define i16 @addm2(i16 %a) #0 {
; CHECK-LABEL: addm2>
; CHECK: add {{r[0-6]}}, -2
    %result = add i16 %a, -2
    ret i16 %result
}

; Should not use aliases
define i16 @cmp1(i16 %a) #0 {
; CHECK-LABEL: cmp1>
; CHECK: cmp {{r[0-6]}}, 1
    %cmp = icmp eq i16 %a, 1
    %result = select i1 %cmp, i16 69, i16 420
    ret i16 %result
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
