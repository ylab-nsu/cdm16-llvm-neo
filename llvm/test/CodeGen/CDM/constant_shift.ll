target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Constant shift larger when 8 bits

; Function Attrs: noinline nounwind optnone
define i16 @shl16(i16 %a) #0 {
entry:
; CHECK-LABEL: shl16_big>
; CHECK: shl r0, r0, 7
  %shl = shl i16 %a, 7
  ret i16 %shl
}

define i16 @shl16_big(i16 %a) #0 {
entry:
; CHECK-LABEL: shl16_big>
; CHECK: shl r0, r0, 8
; CHECK-NEXT: shl r0, r0, 7
  %shl = shl i16 %a, 15
  ret i16 %shl
}

define i16 @shra16_big(i16 %a) #0 {
entry:
; CHECK-LABEL: shra16_big>
; CHECK: shra r0, r0, 8
; CHECK-NEXT: shra r0, r0, 7
  %shra = ashr i16 %a, 15
  ret i16 %shra
}

define i32 @shl32_1(i32 %a) #0 {
entry:
; CHECK-LABEL: shl32_1>
; CHECK: shl r0, r0, 1
; CHECK-NEXT: rcl r1, r1, 1
  %shl = shl i32 %a, 1
  ret i32 %shl
}

define i32 @shra32_1(i32 %a) #0 {
entry:
; CHECK-LABEL: shra32_1>
; CHECK: shra r0, r0, 1
; CHECK-NEXT: rcl r1, r1, 1
  %shra = ashr i32 %a, 1
  ret i32 %shra
}

define i32 @shl32_const(i32 %a) #0 {
entry:
; CHECK-LABEL: shl32_const>
; CHECK: shl r1, r1, 2
; CHECK-NEXT: shr r0, r2, 8
; CHECK-NEXT: shr r2, r2, 6
; CHECK-NEXT: or r1, r2, r1
; CHECK-NEXT: shl r0, r0, 2
  %shl = shl i32 %a, 2
  ret i32 %shl
}

define i32 @shra32_const(i32 %a) #0 {
entry:
; CHECK-LABEL: shra32_const>
; CHECK: shr r0, r0, 2
; CHECK-NEXT: shl r1, r2, 8
; CHECK-NEXT: shl r2, r2, 6
; CHECK-NEXT: or r0, r2, r0
; CHECK-NEXT: shra r1, r1, 2
  %shra = ashr i32 %a, 2
  ret i32 %shra
}

define i64 @shl64_1(i64 %a) #0 {
entry:
; CHECK-LABEL: shl64_1>
; CHECK: shl r0, r0, 1
; CHECK-NEXT: rcl r1, r1, 1
; CHECK-NEXT: rcl r2, r2, 1
; CHECK-NEXT: rcl r3, r3, 1
  %shl = shl i64 %a, 1
  ret i64 %shl
}

define i64 @shra64_1(i64 %a) #0 {
entry:
; CHECK-LABEL: shra64_1>
; CHECK: shra r3, r3, 1
; CHECK-NEXT: rcr r2, r2, 1
; CHECK-NEXT: rcr r1, r1, 1
; CHECK-NEXT: rcr r0, r0, 1
  %shra = ashr i64 %a, 1
  ret i64 %shra
}

define i64 @shl64_const(i64 %a) #0 {
entry:
; CHECK-LABEL: shl64_const>
; CHECK: shr r0, r4, 8
; CHECK-NEXT: shr r4, r4, 6
; CHECK-NEXT: shl r1, r5, 2
; CHECK-NEXT: or r5, r4, r4
; CHECK-NEXT: shr r1, r1, 8
; CHECK-NEXT: shr r1, r1, 6
; CHECK-NEXT: shl r2, r5, 2
; CHECK-NEXT: or r5, r1, r5
; CHECK-NEXT: shr r2, r1, 8
; CHECK-NEXT: shr r1, r1, 6
; CHECK-NEXT: shl r3, r2, 2
; CHECK-NEXT: or r2, r1, r3
; CHECK-NEXT: shl r0, r0, 2
; CHECK-NEXT: move r4, r1
; CHECK-NEXT: move r5, r2
  %shl = shl i64 %a, 2
  ret i64 %shl
}

define i64 @shra64_const(i64 %a) #0 {
entry:
; CHECK-LABEL: shra64_const>
; CHECK: shr r0, r0, 2
; CHECK-NEXT: shl r1, r4, 8
; CHECK-NEXT: shl r4, r4, 6
; CHECK-NEXT: or r0, r4, r0
; CHECK-NEXT: shr r1, r1, 2
; CHECK-NEXT: shl r2, r4, 8
; CHECK-NEXT: shl r4, r4, 6
; CHECK-NEXT: or r1, r4, r1
; CHECK-NEXT: shr r2, r2, 2
; CHECK-NEXT: shl r3, r4, 8
; CHECK-NEXT: shl r4, r4, 6
; CHECK-NEXT: or r2, r4, r2
; CHECK-NEXT: shra r3, r3, 2
  %shra = ashr i64 %a, 2
  ret i64 %shra
}

attributes #0 = { noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
