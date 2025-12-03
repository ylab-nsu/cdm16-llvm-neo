target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for variadic functions:
; - Caller passes arguments as usual
; - Callee stores all registers r0-r3 (except ones, what hold regular arguments) in "shadow space" on stack

define i16 @foo(i16 %a, ...) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -2
; CHECK-NEXT: ssw r3, 10
; CHECK-NEXT: ssw r2, 8
; CHECK-NEXT: ssw r1, 6
; CHECK-NEXT: add r1, r0, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %list = alloca ptr, align 2
  call void @llvm.lifetime.start.p0(i64 2, ptr nonnull %list) #4
  call void @llvm.va_start.p0(ptr nonnull %list)
  %0 = va_arg ptr %list, i16
  call void @llvm.va_end.p0(ptr %list)
  %add = add nsw i16 %0, %a
  call void @llvm.lifetime.end.p0(i64 2, ptr nonnull %list) #4
  ret i16 %add
}

declare void @llvm.lifetime.start.p0(i64 immarg, ptr captures(none)) #1
declare void @llvm.va_start.p0(ptr) #2
declare void @llvm.va_end.p0(ptr) #2
declare void @llvm.lifetime.end.p0(i64 immarg, ptr captures(none)) #1

define i16 @bar() #3 {
entry:
; CHECK-LABEL: bar>
; CHECK: addsp -10
; CHECK-NEXT: ldsp [[RD1:r[0-6]]]
; CHECK-NEXT: ldi [[RD2:r[0-6]]], 8
; CHECK-NEXT: ldi [[RS:r[0-6]]], 5
; CHECK-NEXT: stw [[RD1]], [[RD2]], [[RS]]
; CHECK-NEXT: ldi r0, 1
; CHECK-NEXT: ldi r1, 2
; CHECK-NEXT: ldi r2, 3
; CHECK-NEXT: ldi r3, 4
; CHECK-NEXT: jsr foo
; CHECK-NEXT: addsp 10
  %call = tail call i16 (i16, ...) @foo(i16 1, i16 2, i16 3, i16 4, i16 5)
  ret i16 %call
}

attributes #0 = { mustprogress nofree noinline norecurse nosync nounwind willreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nosync nounwind willreturn }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #4 = { nounwind }
