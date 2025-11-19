target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for basic calling convention
; - Passing argument on register
; - Saving/restoring fp and sp
; - Allocation of "shadow space"

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none)
define i16 @foo(i16 %a) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: add r0, 2
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %add = add nsw i16 %a, 2
  ret i16 %add
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i16 @main() #0 {
entry:
; CHECK-LABEL: main>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -8
; CHECK-NEXT: ldi r0, 2
; CHECK-NEXT: jsr foo
; CHECK-NEXT: addsp 8
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %call = tail call i16 @foo(i16 2)
  ret i16 %call
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
