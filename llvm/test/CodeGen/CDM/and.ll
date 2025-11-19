target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for bitwise and operator

define i8 @and8_reg_reg(i8 %a, i8 %b) {
; CHECK-LABEL: and8_reg_reg>
; CHECK: and r0, r1, r0
    %result = and i8 %a, %b
    ret i8 %result
}

define i8 @and8_reg_imm(i8 %a) {
; CHECK-LABEL: and8_reg_imm>
; CHECK: ldi [[REG:r[0-6]]], 5
; CHECK: and r0, [[REG]], r0
    %result = and i8 %a, 5
    ret i8 %result
}

define i16 @and16_reg_reg(i16 %a, i16 %b) {
; CHECK-LABEL: and16_reg_reg>
; CHECK: and r0, r1, r0
    %result = and i16 %a, %b
    ret i16 %result
}

define i16 @and16_reg_imm(i16 %a) {
; CHECK-LABEL: and16_reg_imm>
; CHECK: ldi [[REG:r[0-6]]], 1234
; CHECK: and r0, [[REG]], r0
    %result = and i16 %a, 1234
    ret i16 %result
}

define i32 @and32_reg_reg(i32 %a, i32 %b) {
; CHECK-LABEL: and32_reg_reg>
; CHECK: and r0, r2, r0
; CHECK: and r1, r3, r1
    %result = and i32 %a, %b
    ret i32 %result
}

define i32 @and32_reg_imm(i32 %a) {
; CHECK-LABEL: and32_reg_imm>
; CHECK: ldi [[REG1:r[0-6]]], -13035
; CHECK: and r0, [[REG1]], r0
; CHECK: ldi [[REG2:r[0-6]]], 1883
; CHECK: and r1, [[REG2]], r1
    %result = and i32 %a, 123456789
    ret i32 %result
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
