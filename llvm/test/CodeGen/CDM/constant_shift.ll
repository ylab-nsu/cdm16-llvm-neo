target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Constant shift larger when 8 bits

; Function Attrs: noinline nounwind optnone
define i16 @foo(i16 %a) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK: shl r0, r0, 7
  %shl = shl i16 %a, 7
  ret i16 %shl
}

attributes #0 = { noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
