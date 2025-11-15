target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none)
define i64 @foo(i64 %a) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -10
; CHECK-NEXT: ssw r4, -2
; CHECK-NEXT: ssw r5, -4
; CHECK-NEXT: ssw r6, -6
; CHECK-NEXT: ssw r3, -8
; CHECK-NEXT: ssw r2, -10
; CHECK-NEXT: move r0, r4
; CHECK-NEXT: add r0, 2
; CHECK-NEXT: ldi r5, 1
; CHECK-NEXT: move r5, r6
; CHECK-NEXT: cmp r0, r4
; CHECK-NEXT: blo __LBB0_2
; CHECK-NEXT: %bb.1:
; CHECK-NEXT: ldi r6, 0
; CHECK-NEXT: _LBB0_2:
; CHECK-NEXT: add r1, r6, r2
; CHECK-NEXT: move r5, r3
; CHECK-NEXT: cmp r2, r1
; CHECK-NEXT: blo __LBB0_4
; CHECK-NEXT: %bb.3:
; CHECK-NEXT: ldi	r3, 0
; CHECK-NEXT: _LBB0_4:
; CHECK-NEXT: cmp r0, r4
; CHECK-NEXT: bhs __LBB0_6
; CHECK-NEXT: %bb.5:
; CHECK-NEXT: move r3, r6
; CHECK-NEXT: _LBB0_6:
; CHECK-NEXT: lsw r1, -10
; CHECK-NEXT: add r1, r6, r4
; CHECK-NEXT: cmp r4, r1
; CHECK-NEXT: blo __LBB0_8
; CHECK-NEXT: %bb.7:
; CHECK-NEXT: ldi r5, 0
; CHECK-NEXT: _LBB0_8:
; CHECK-NEXT: lsw r1, -8
; CHECK-NEXT: add r1, r5, r3
; CHECK-NEXT: move r2, r1
; CHECK-NEXT: move r4, r2
; CHECK-NEXT: lsw r6, -6
; CHECK-NEXT: lsw r5, -4
; CHECK-NEXT: lsw r4, -2
; CHECK-NEXT: addsp 10
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %add = add nsw i64 %a, 2
  ret i64 %add
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
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
