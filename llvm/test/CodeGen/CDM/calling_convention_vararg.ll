target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Function Attrs: nofree noinline norecurse nosync nounwind
define i16 @foo(i16 %amount, ...) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -4
; CHECK-NEXT: ssw r4, -2
; CHECK-NEXT: ssw r3, 10
; CHECK-NEXT: ssw r2, 8
; CHECK-NEXT: ssw r1, 6
; CHECK-NEXT: ldi r1, 0
; CHECK-NEXT: cmp r0, 1
; CHECK-NEXT: blt __LBB0_3
; CHECK-NEXT: br __LBB0_1
; CHECK-NEXT: __LBB0_1
; CHECK-NEXT: add r0, 1
; CHECK-NEXT: ldi r2, -2
; CHECK-NEXT: __LBB0_2
; CHECK: lsw r3, -4
; CHECK-NEXT: add r3, 1
; CHECK-NEXT: and r3, r2, r3
; CHECK-NEXT: move r3, r4
; CHECK-NEXT: add r4, 2
; CHECK-NEXT: ssw r4, -4
; CHECK-NEXT: ldw r3, r3
; CHECK-NEXT: add r3, r1, r1
; CHECK-NEXT: add r0, -1
; CHECK-NEXT: cmp r0, 1
; CHECK-NEXT: bgt __LBB0_2
; CHECK-NEXT: br __LBB0_3
; CHECK-NEXT: __LBB0_3
; CHECK-NEXT: move r1, r0
; CHECK-NEXT: lsw r4, -2
; CHECK-NEXT: addsp 4
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %args = alloca ptr, align 2
  %cmp2 = icmp sgt i16 %amount, 0
  br i1 %cmp2, label %while.body, label %while.end

while.body:
  %amount.addr.04 = phi i16 [ %dec, %while.body ], [ %amount, %entry ]
  %sum.03 = phi i16 [ %add, %while.body ], [ 0, %entry ]
  %dec = add nsw i16 %amount.addr.04, -1
  %0 = va_arg ptr %args, i16
  %add = add nsw i16 %0, %sum.03
  %cmp = icmp sgt i16 %amount.addr.04, 1
  br i1 %cmp, label %while.body, label %while.end

while.end:
  %sum.0.lcssa = phi i16 [ 0, %entry ], [ %add, %while.body ]
  ret i16 %sum.0.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind
define i16 @bar() #0 {
entry:
; CHECK-LABEL: bar>
; CHECK: ldi r0, 2
; CHECK-NEXT: ldi r1, 1
; CHECK-NEXT: ldi r2, 3
; CHECK-NEXT: jsr foo
  %call = tail call i16 (i16, ...) @foo(i16 noundef 2, i16 noundef 1, i16 noundef 3)
  ret i16 %call
}

attributes #0 = { nofree noinline norecurse nosync nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
