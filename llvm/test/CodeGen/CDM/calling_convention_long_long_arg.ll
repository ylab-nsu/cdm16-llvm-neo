target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for passing i64 argument on registers r0-r3

define i64 @foo(i64 %a) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: ldi r1, 1
; CHECK-NEXT: and r0, r1, r0
; CHECK-NEXT: ldi r1, 0
; CHECK-NEXT: move r1, r2
; CHECK-NEXT: move r1, r3
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %and = and i64 %a, 1
  ret i64 %and
}

define i64 @bar() #0 {
entry:
; CHECK-LABEL: bar>
; CHECK: ldi r0, 2
; CHECK-NEXT: ldi r1, 0
; CHECK-NEXT: move r1, r2
; CHECK-NEXT: move r1, r3
; CHECK-NEXT: jsr foo
  %call = tail call i64 @foo(i64 2)
  ret i64 %call
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
