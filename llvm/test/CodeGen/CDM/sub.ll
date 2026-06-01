target datalayout = "e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-n16-S16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

; Tests for substruction operator

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i8 @sub8(i8 signext %a, i8 signext %b) #0 {
entry:
; CHECK-LABEL: sub8>
; CHECK: sub r0, r1, r0
; CHECK-NEXT: sxt r0, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i8 %a, %b
  ret i8 %sub
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i16 @sub16(i16 %a, i16 %b) #0 {
entry:
; CHECK-LABEL: sub16>
; CHECK: sub r0, r1, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i16 %a, %b
  ret i16 %sub
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
