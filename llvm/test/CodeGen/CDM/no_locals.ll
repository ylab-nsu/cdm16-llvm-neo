target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for function without locals and any subsequent calls

; Function Attrs: norecurse nosync nounwind willreturn memory(none)
define i16 @foo() #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NOT: addsp
  ret i16 0
}

attributes #0 = { norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
