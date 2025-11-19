target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Tests for shifts with variable

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i8 @shift8(i8 signext %a, i8 signext %b) #0 {
entry:
; CHECK-LABEL: shift8>
; CHECK: br [[LABEL2:.*]]
; CHECK-NEXT: [[LABEL1:.*]]:
; CHECK: shra r0, r0, 1
; CHECK-NEXT: sub r1, 1
; CHECK-NEXT: [[LABEL2]]:
; CHECK: cmp r1, 0
; CHECK-NEXT: bgt [[LABEL1]]
  %conv = sext i8 %a to i16
  %conv13 = zext nneg i8 %b to i16
  %shr = ashr i16 %conv, %conv13
  %conv2 = trunc i16 %shr to i8
  ret i8 %conv2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i16 @shift16(i16 %a, i16 %b) #0 {
entry:
; CHECK-LABEL: shift16>
; CHECK: br [[LABEL2:.*]]
; CHECK-NEXT: [[LABEL1:.*]]:
; CHECK: shra r0, r0, 1
; CHECK-NEXT: sub r1, 1
; CHECK-NEXT: [[LABEL2]]:
; CHECK: cmp r1, 0
; CHECK-NEXT: bgt [[LABEL1]]
  %shr = ashr i16 %a, %b
  ret i16 %shr
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i32 @shift32(i32 %a, i32 %b) #0 {
entry:
; CHECK-LABEL: shift32>
; CHECK: br [[LABEL2:.*]]
; CHECK-NEXT: [[LABEL1:.*]]:
; CHECK: shra r1, r1, 1
; CHECK-NEXT: rcr r0, r0, 1
; CHECK-NEXT: sub r2, 1
; CHECK-NEXT: [[LABEL2]]:
; CHECK: cmp r2, 0
; CHECK-NEXT: bgt [[LABEL1]]
  %shr = ashr i32 %a, %b
  ret i32 %shr
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i64 @shift64(i64 %a, i64 %b) #0 {
entry:
; CHECK-LABEL: shift64>
; CHECK: lsw [[REG:r[0-6]]], 12
; CHECK-NEXT: br [[LABEL2:.*]]
; CHECK-NEXT: [[LABEL1:.*]]:
; CHECK: shra r3, r3, 1
; CHECK-NEXT: rcr r2, r2, 1
; CHECK-NEXT: rcr r1, r1, 1
; CHECK-NEXT: rcr r0, r0, 1
; CHECK-NEXT: sub [[REG]], 1
; CHECK-NEXT: [[LABEL2]]:
; CHECK: cmp [[REG]], 0
; CHECK-NEXT: bgt [[LABEL1]]
  %shr = ashr i64 %a, %b
  ret i64 %shr
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
