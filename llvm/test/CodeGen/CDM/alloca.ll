target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for allocations on function stack frame

define void @alloca_addressof_small() #0 {
entry:
; CHECK-LABEL: alloca_addressof_small>
; CHECK: addsp -36
  %p = alloca [18 x i16]
  ret void
}

define void @alloca_addressof_big() #0 {
entry:
; CHECK-LABEL: alloca_addressof_big>
; TODO: Add check for proper SP adjustment
  %p = alloca [513 x i16]
  ret void
}

attributes #0 = { noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
