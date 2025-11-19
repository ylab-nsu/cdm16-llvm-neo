target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for passing 5th argument on stack

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none)
define i16 @foo(i16 %a, i16 %b, i16 %c, i16 %d, i16 %e) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp

; CHECK-NEXT: add r0, r1, r0
; CHECK-NEXT: add r0, r2, r0
; CHECK-NEXT: add r0, r3, r0
; CHECK-NEXT: lsw [[REG:r[0-6]]], 12
; CHECK-NEXT: add r0, [[REG]], r0

; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %add1 = add i16 %a, %b
  %add2 = add i16 %add1, %c
  %add3 = add i16 %add2, %d
  %add4 = add i16 %add3, %e
  ret i16 %add4
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i16 @main() #0 {
entry:
; CHECK-LABEL: main>
; CHECK: addsp -10
; CHECK-NEXT: ldsp [[RD1:r[0-6]]]
; CHECK-NEXT: ldi [[RD2:r[0-6]]], 8
; CHECK-NEXT: ldi [[RS:r[0-6]]], 6
; CHECK-NEXT: stw [[RD1]], [[RD2]], [[RS]]
; CHECK-NEXT: ldi r0, 2
; CHECK-NEXT: ldi r1, 3
; CHECK-NEXT: ldi r2, 4
; CHECK-NEXT: ldi r3, 5
; CHECK-NEXT: jsr foo
; CHECK-NEXT: addsp 10
  %call = tail call i16 @foo(i16 2, i16 3, i16 4, i16 5, i16 6)
  ret i16 %call
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

