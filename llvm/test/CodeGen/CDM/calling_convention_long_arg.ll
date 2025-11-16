target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none)
define i32 @foo(i32 %a) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: ldi r1, 2
; CHECK-NEXT: and r0, r1, r0
; CHECK-NEXT: ldi r1, 0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %add = and i32 %a, 2
  ret i32 %add
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i32 @bar() #0 {
entry:
; CHECK-LABEL: bar>
; CHECK: ldi r0, 2
; CHECK-NEXT: ldi r1, 0
; CHECK-NEXT: jsr foo
  %call = tail call i32 @foo(i32 2)
  ret i32 %call
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
