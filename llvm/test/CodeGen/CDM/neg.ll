target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i16 @neg16(i16 %a) #0 {
entry:
; CHECK-LABEL: neg16>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: neg r0, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i16 0, %a
  ret i16 %sub
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i8 @neg8(i8 signext %a) #0 {
entry:
; CHECK-LABEL: neg8>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: neg r0, r0
; CHECK-NEXT: sxt r0, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i8 0, %a
  ret i8 %sub
}

define i32 @neg32(i32 %a) #0 {
entry:
; CHECK-LABEL: neg32>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: ldi r2, 1
; CHECK-NEXT: cmp r0, 0
; CHECK-NEXT: bne __LBB2_2
; CHECK-NEXT: # %bb.1:
; CHECK-NEXT: ldi r2, 0
; CHECK-NEXT: __LBB2_2
; CHECK-NEXT: add r1, r2, r1
; CHECK-NEXT: neg r1, r1
; CHECK-NEXT: neg r0, r0
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i32 0, %a
  ret i32 %sub
}

define i64 @neg64(i64 %a) #0 {
entry:	
; CHECK-LABEL: neg64>
; CHECK-NEXT: # %bb.0:
; CHECK-NEXT: push fp
; CHECK-NEXT: ldsp fp
; CHECK-NEXT: addsp -8
; CHECK-NEXT: ssw r4, -2
; CHECK-NEXT: ssw r5, -4
; CHECK-NEXT: ssw r6, -6
; CHECK-NEXT: ssw r3, -8
; CHECK-NEXT: ldi r6, 0
; CHECK-NEXT: ldi r4, 1
; CHECK-NEXT: move r4, r3
; CHECK-NEXT: cmp r1, 0
; CHECK-NEXT: bne __LBB3_2
; CHECK-NEXT: # %bb.1:
; CHECK-NEXT: move r6, r3
; CHECK-NEXT: __LBB3_2
; CHECK-NEXT: move r4, r5
; CHECK-NEXT: cmp r0, 0
; CHECK-NEXT: bne __LBB3_4
; CHECK-NEXT: # %bb.3:
; CHECK-NEXT: move r6, r5
; CHECK-NEXT: __LBB3_4
; CHECK-NEXT: move r5, r6
; CHECK-NEXT: cmp r1, 0
; CHECK-NEXT: beq __LBB3_6
; CHECK-NEXT: # %bb.5:
; CHECK-NEXT: move r3, r6
; CHECK-NEXT: __LBB3_6
; CHECK-NEXT: move r4, r3
; CHECK-NEXT: cmp r2, 0
; CHECK-NEXT: bne __LBB3_8
; CHECK-NEXT: # %bb.7:
; CHECK-NEXT: ldi r3, 0
; CHECK-NEXT: __LBB3_8
; CHECK-NEXT: neg r2, r2
; CHECK-NEXT: cmp r2, r6
; CHECK-NEXT: blo __LBB3_10
; CHECK-NEXT: # %bb.9:
; CHECK-NEXT: ldi r4, 0
; CHECK-NEXT: __LBB3_10
; CHECK-NEXT: sub r2, r6, r2
; CHECK-NEXT: lsw r6, -8
; CHECK-NEXT: add r6, r3, r3
; CHECK-NEXT: add r3, r4, r3
; CHECK-NEXT: add r1, r5, r1
; CHECK-NEXT: neg r3, r3
; CHECK-NEXT: neg r1, r1
; CHECK-NEXT: neg r0, r0
; CHECK-NEXT: lsw r6, -6
; CHECK-NEXT: lsw r5, -4
; CHECK-NEXT: lsw r4, -2
; CHECK-NEXT: addsp 8
; CHECK-NEXT: stsp fp
; CHECK-NEXT: pop fp
; CHECK-NEXT: rts
  %sub = sub i64 0, %a
  ret i64 %sub
}


attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
