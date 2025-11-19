target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for struct by val returning:
; - Caller allocates struct on stack
; - Caller passes pointer to it as 0-th argument
; - Callee stores returned struct value in struct by this pointer

%struct.s = type { i16, i16 }

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(argmem: write)
define void @foo(ptr dead_on_unwind noalias writable writeonly sret(%struct.s) align 2 captures(none) initializes((0, 4)) %agg.result, i16 noundef %a, i16 noundef %b) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: ldi [[REG:r[0-6]]], 2
; CHECK-NEXT: stw r0, [[REG]], r2
; CHECK-NEXT: stw r0, r1
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  store i16 %a, ptr %agg.result, align 2
  %b2 = getelementptr inbounds nuw i8, ptr %agg.result, i16 2
  store i16 %b, ptr %b2, align 2
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define dso_local i16 @bar() #1 {
entry:
; CHECK-LABEL: bar>
; CHECK: addsp -8

; CHECK-NEXT: ssw r4, -2
; CHECK-NEXT: ssw r5, -4

; CHECK-NEXT: addsp -8
; CHECK-NEXT: ldi [[REG1:r[0-6]]], -8
; CHECK-NEXT: add [[REG1]], fp, [[PTRREG:r[0-6]]]
; CHECK-NEXT: ldi [[REG2:r[0-6]]], 2
; CHECK-NEXT: ldi r2, 3
; CHECK-NEXT: move [[PTRREG]], r0
; CHECK-NEXT: move [[REG2]], r1
; CHECK-NEXT: jsr foo
; CHECK-NEXT: addsp 8
  %out = alloca %struct.s, align 2
  call void @foo(ptr dead_on_unwind nonnull writable sret(%struct.s) align 2 %out, i16 noundef 2, i16 noundef 3)
  %0 = load i16, ptr %out, align 2
  %b = getelementptr inbounds nuw i8, ptr %out, i16 2
  %1 = load i16, ptr %b, align 2
  %add = add i16 %1, %0
  ret i16 %add
}

attributes #0 = { mustprogress nofree noinline norecurse nosync nounwind willreturn memory(argmem: write) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
