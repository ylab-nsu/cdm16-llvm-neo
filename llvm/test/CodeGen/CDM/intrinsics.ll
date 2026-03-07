target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

; CDM-specific LLVM intrinsics

define void @halt() {
; CHECK-LABEL: halt>
; CHECK: halt
    call void @llvm.cdm.halt()
    ret void
}

define void @wait() {
; CHECK-LABEL: wait>
; CHECK: wait
    call void @llvm.cdm.wait()
    ret void
}

define void @ei() {
; CHECK-LABEL: ei>
; CHECK: ei
    call void @llvm.cdm.ei()
    ret void
}

define void @di() {
; CHECK-LABEL: di>
; CHECK: di
    call void @llvm.cdm.di()
    ret void
}

define void @int() {
; CHECK-LABEL: int>
; CHECK: int 69
    call void @llvm.cdm.int(i16 69)
    ret void
}

define void @reset() {
; CHECK-LABEL: reset>
; CHECK: reset 420
    call void @llvm.cdm.reset(i16 420)
    ret void
}

define i16 @ldps() {
; CHECK-LABEL: ldps>
; CHECK: ldps {{r[0-6]}}
    %res = call i16 @llvm.cdm.ldps()
    ret i16 %res
}

define void @stps(i16 %val) {
; CHECK-LABEL: stps>
; CHECK: stps {{r[0-6]}}
    call void @llvm.cdm.stps(i16 %val)
    ret void
}

