target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for calling function with function pointer

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none)
define i16 @foo(i16 %a) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: add r0, 1
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %add = add i16 %a, 1
  ret i16 %add
}

; Function Attrs: noinline nounwind
define i16 @apply(i16 %a, ptr readonly captures(none) %f) #1 {
entry:
; CHECK-LABEL: apply>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -8
; CHECK-NEXT: jsrr r1
; CHECK-NEXT: addsp 8
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %call = tail call i16 %f(i16 %a) #3
  ret i16 %call
}

; Function Attrs: nounwind
define i16 @bar() #2 {
entry:
; CHECK-LABEL: bar>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -8
; CHECK-NEXT: ldi r0, 2
; CHECK-NEXT: ldi r1, foo
; CHECK-NEXT: jsr apply
; CHECK-NEXT: addsp 8
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %call = tail call i16 @apply(i16 2, ptr nonnull @foo)
  ret i16 %call
}

attributes #0 = { mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { noinline nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #2 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #3 = { nounwind }
