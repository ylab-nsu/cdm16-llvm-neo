target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

define signext i8 @add8_reg_reg(i8 signext %a, i8 signext %b) #0 {
; CHECK-LABEL: add8_reg_reg>
; CHECK: add r0, r1, r0
; CHECK-NEXT: sxt r0, r0
    %result = add i8 %a, %b
    ret i8 %result
}

define i16 @add16_reg_reg(i16 %a, i16 %b) #0 {
; CHECK-LABEL: add16_reg_reg>
; CHECK: add r0, r1, r0
    %result = add i16 %a, %b
    ret i16 %result
}

define i16 @add16_reg_imm(i16 %a) #0 {
; CHECK-LABEL: add16_reg_imm>
; CHECK: add r0, 63
    %result = add i16 %a, 63
    ret i16 %result
}

define i32 @add32_reg_reg(i32 %a, i32 %b) #0 {
; CHECK-LABEL: add32_reg_reg>
; CHECK: move r0, r4
; CHECK-NEXT: add r4, r2, r0
; CHECK-NEXT: ldi r2, 1
; CHECK-NEXT: cmp r0, r4
; CHECK-NEXT: blo __LBB3_2
; CHECK-NEXT: # %bb.1:
; CHECK-NEXT: ldi r2, 0
; CHECK-NEXT: __LBB3_2:
; CHECK-NEXT: add r1, r3, r1
; CHECK-NEXT: add r1, r2, r1
    %result = add i32 %a, %b
    ret i32 %result
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
