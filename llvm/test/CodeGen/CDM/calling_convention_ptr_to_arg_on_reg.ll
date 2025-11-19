target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for storing argument, passed in register in "shadow space", caused by taking pointer to it

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @bar(ptr %a) #0 {
entry:
  %0 = load i16, ptr %a, align 2
  %inc = add i16 %0, 1
  store i16 %inc, ptr %a, align 2
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i16 @foo(i16 %a) #1 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -2
; CHECK-NEXT: ssw r0, -2
; CHECK-NEXT: addsp -8
; CHECK-NEXT: ldi r0, -2
; CHECK-NEXT: add r0, fp, r0
; CHECK-NEXT: jsr bar
; CHECK-NEXT: addsp 8
; CHECK-NEXT: lsw r0, -2
; CHECK-NEXT: add r0, 1
; CHECK-NEXT: addsp 2
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %a.addr = alloca i16, align 2
  store i16 %a, ptr %a.addr, align 2
  call void @bar(ptr nonnull %a.addr)
  %0 = load i16, ptr %a.addr, align 2
  %add = add nsw i16 %0, 1
  ret i16 %add
}

attributes #0 = { mustprogress nofree noinline norecurse nosync nounwind willreturn memory(argmem: readwrite) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
