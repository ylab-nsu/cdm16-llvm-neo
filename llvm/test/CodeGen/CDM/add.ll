target datalayout = "e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-n16-S16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

; Test for addition operator testing

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

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
