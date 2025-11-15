target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

define i64 @foo(i64 %a, i16 %b) #0 {
entry:
  %add = add i64 %a, 2
  ret i64 %add
}

define i64 @bar() #0 {
entry:
; CHECK-LABEL: bar>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -16
; CHECK-NEXT: ldsp r0
; CHECK-NEXT: ldi r1, 10
; CHECK-NEXT: ldi r2, 1883
; CHECK-NEXT: stw r0, r1, r2
; CHECK-NEXT: ldi r1, 8
; CHECK-NEXT: ldi r2, -13035
; CHECK-NEXT: stw r0, r1, r2
; CHECK-NEXT: ldi r2, 14
; CHECK-NEXT: ldi r1, 0
; CHECK-NEXT: stw r0, r2, r1
; CHECK-NEXT: ldi r2, 12
; CHECK-NEXT: stw r0, r2, r1
; CHECK-NEXT: ldi r0, 2
; CHECK-NEXT: move r1, r2
; CHECK-NEXT: move r1, r3
; CHECK-NEXT: jsr foo
; CHECK-NEXT: addsp 16
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts

  %call = tail call i64 @foo(i64 2, i64 123456789)
  ret i64 %call
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
