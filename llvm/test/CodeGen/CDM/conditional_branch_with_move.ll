target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Regression test for so-called "move problem":
; - cmp sets arithmetic flags in PS register
; - Conditional branch uses these flags 
; BUT:
; - Move used to be inserted between cmp and conditional branch, so it clobbers PS register

define i16 @foo(i16 %a, i16 %b, i16 %c, i16 %d) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK: move r1, r3
; CHECK-NEXT: cmp r0, r1
; CHECK-NEXT: beq {{.*}}
  %cmp = icmp eq i16 %a, %b
  %cmp1 = icmp eq i16 %b, %c
  %c.d = select i1 %cmp1, i16 %c, i16 %d
  %ret.0 = select i1 %cmp, i16 %b, i16 %c.d
  %add = add i16 %ret.0, %b
  ret i16 %add
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
