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

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i64 @add64(i64 %a, i64 %b) #0 {
entry:
; CHECK-LABEL: add64>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -12
; CHECK-NEXT: ssw r4, -2
; CHECK-NEXT: ssw r5, -4
; CHECK-NEXT: ssw r6, -6
; CHECK-NEXT: ssw r3, -8
; CHECK-NEXT: ssw r2, -12
; CHECK-NEXT: lsw r2, 12
; CHECK-NEXT: add r2, r0, r0
; CHECK-NEXT: ldi r3, 0
; CHECK-NEXT: ldi r5, 1
; CHECK-NEXT: move r5, r4
; CHECK-NEXT: ssw r0, -10
; CHECK-NEXT: cmp r0, r2
; CHECK-NEXT: blo __LBB4_2
; CHECK-NEXT: # %bb.1:
; CHECK-NEXT: move r3, r4
; CHECK-NEXT: __LBB4_2
; CHECK-NEXT: ldi r2, 12
; CHECK-NEXT: add r2, fp, r0
; CHECK-NEXT: ldi r2, 2
; CHECK-NEXT: ldw r0, r2, r6
; CHECK-NEXT: add r6, r1, r1
; CHECK-NEXT: add r1, r4, r1
; CHECK-NEXT: move r5, r2
; CHECK-NEXT: cmp r1, r6
; CHECK-NEXT: blo __LBB4_4
; CHECK-NEXT: # %bb.3:
; CHECK-NEXT: move r3, r2
; CHECK-NEXT: __LBB4_4
; CHECK-NEXT: cmp r1, r6
; CHECK-NEXT: beq __LBB4_6
; CHECK-NEXT: # %bb.5:
; CHECK-NEXT: move r2, r4
; CHECK-NEXT: __LBB4_6
; CHECK-NEXT: ldi r2, 4
; CHECK-NEXT: ldw r0, r2, r3
; CHECK-NEXT: lsw r2, -12
; CHECK-NEXT: add r3, r2, r6
; CHECK-NEXT: add r6, r4, r2
; CHECK-NEXT: move r5, r4
; CHECK-NEXT: cmp r6, r3
; CHECK-NEXT: blo __LBB4_8
; CHECK-NEXT: # %bb.7:
; CHECK-NEXT: ldi r4, 0
; CHECK-NEXT: __LBB4_8
; CHECK-NEXT: cmp r2, r6
; CHECK-NEXT: blo __LBB4_10
; CHECK-NEXT: # %bb.9:
; CHECK-NEXT: ldi r5, 0
; CHECK-NEXT: __LBB4_10
; CHECK-NEXT: ldi r3, 6
; CHECK-NEXT: ldw r0, r3, r0
; CHECK-NEXT: lsw r3, -8
; CHECK-NEXT: add r0, r3, r0
; CHECK-NEXT: add r0, r4, r0
; CHECK-NEXT: add r0, r5, r3
; CHECK-NEXT: lsw r0, -10
; CHECK-NEXT: lsw r6, -6
; CHECK-NEXT: lsw r5, -4
; CHECK-NEXT: lsw r4, -2
; CHECK-NEXT: addsp 12
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %add = add i64 %b, %a
  ret i64 %add
}

attributes #0 = { noinline norecurse nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
