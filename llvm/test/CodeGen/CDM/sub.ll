target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i8 @sub8(i8 signext %a, i8 signext %b) #0 {
entry:
; CHECK-LABEL: sub8>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: sub r0, r1, r0
; CHECK-NEXT: sxt r0, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i8 %a, %b
  ret i8 %sub
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i16 @sub16(i16 %a, i16 %b) #0 {
entry:
; CHECK-LABEL: sub16>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: sub r0, r1, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i16 %a, %b
  ret i16 %sub
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i32 @sub32(i32 %a, i32 %b) #0 {
entry:
; CHECK-LABEL: sub32>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -2
; CHECK-NEXT: ssw r4, -2
; CHECK-NEXT: ldi r4, 1
; CHECK-NEXT: cmp r0, r2
; CHECK-NEXT: blo __LBB2_2
; CHECK-NEXT: # %bb.1:
; CHECK-NEXT: ldi r4, 0
; CHECK-NEXT: __LBB2_2
; CHECK-NEXT: sub r1, r3, r1
; CHECK-NEXT: sub r1, r4, r1
; CHECK-NEXT: sub r0, r2, r0
; CHECK-NEXT: lsw r4, -2
; CHECK-NEXT: addsp 2
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i32 %a, %b
  ret i32 %sub
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i64 @sub64(i64 %a, i64 %b) #0 {
entry:
; CHECK-LABEL: sub64>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -14
; CHECK-NEXT: ssw r4, -2
; CHECK-NEXT: ssw r5, -4
; CHECK-NEXT: ssw r6, -6
; CHECK-NEXT: ssw r3, -12
; CHECK-NEXT: ldi r3, 12
; CHECK-NEXT: add r3, fp, r4
; CHECK-NEXT: ldi r3, 2
; CHECK-NEXT: ssw r4, -8
; CHECK-NEXT: ldw r4, r3, r3
; CHECK-NEXT: ldi r6, 1
; CHECK-NEXT: lsw r5, 12
; CHECK-NEXT: move r6, r4
; CHECK-NEXT: ssw r0, -10
; CHECK-NEXT: ssw r5, -14
; CHECK-NEXT: cmp r0, r5
; CHECK-NEXT: blo __LBB3_2
; CHECK-NEXT: # %bb.1:
; CHECK-NEXT: ldi r4, 0
; CHECK-NEXT: __LBB3_2
; CHECK-NEXT: move r6, r0
; CHECK-NEXT: cmp r1, r3
; CHECK-NEXT: blo __LBB3_4
; CHECK-NEXT: # %bb.3:
; CHECK-NEXT: ldi r0, 0
; CHECK-NEXT: __LBB3_4
; CHECK-NEXT: move r4, r5
; CHECK-NEXT: cmp r1, r3
; CHECK-NEXT: beq __LBB3_6
; CHECK-NEXT: # %bb.5:
; CHECK-NEXT: move r0, r5
; CHECK-NEXT: __LBB3_6
; CHECK-NEXT: sub r1, r3, r1
; CHECK-NEXT: ldi r0, 4
; CHECK-NEXT: lsw r3, -8
; CHECK-NEXT: ldw r3, r0, r3
; CHECK-NEXT: move r6, r0
; CHECK-NEXT: cmp r2, r3
; CHECK-NEXT: blo __LBB3_8
; CHECK-NEXT: # %bb.7:
; CHECK-NEXT: ldi r0, 0
; CHECK-NEXT: __LBB3_8
; CHECK-NEXT: sub r1, r4, r1
; CHECK-NEXT: sub r2, r3, r2
; CHECK-NEXT: cmp r2, r5
; CHECK-NEXT: blo __LBB3_10
; CHECK-NEXT: # %bb.9:
; CHECK-NEXT: ldi r6, 0
; CHECK-NEXT: __LBB3_10
; CHECK-NEXT: sub r2, r5, r2
; CHECK-NEXT: ldi r3, 6
; CHECK-NEXT: lsw r4, -8
; CHECK-NEXT: ldw r4, r3, r3
; CHECK-NEXT: lsw r4, -12
; CHECK-NEXT: sub r4, r3, r3
; CHECK-NEXT: sub r3, r0, r0
; CHECK-NEXT: sub r0, r6, r3
; CHECK-NEXT: lsw r0, -10
; CHECK-NEXT: lsw r4, -14
; CHECK-NEXT: sub r0, r4, r0
; CHECK-NEXT: lsw r6, -6
; CHECK-NEXT: lsw r5, -4
; CHECK-NEXT: lsw r4, -2
; CHECK-NEXT: addsp 14
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i64 %a, %b
  ret i64 %sub
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
