target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -O0 -mtriple=cdm < %s | FileCheck %s

; Test emission of builtin function calls.

@array = dso_local global [100 x i8] zeroinitializer, align 1

define void @call_memcpy() #0 {
    ; CHECK-LABEL: call_memcpy>
    ; CHECK: jsr memcpy
    %buf = alloca [100 x i8], align 1
    call void @llvm.memcpy.p0.p0.i16(ptr %buf, ptr @array, i16 100, i1 0)
    ret void
}

define void @call_memmove() #0 {
    ; CHECK-LABEL: call_memmove>
    ; CHECK: jsr memmove
    %buf = alloca [100 x i8], align 1
    call void @llvm.memmove.p0.p0.i16(ptr %buf, ptr @array, i16 100, i1 0)
    ret void
}

define i16 @call_mulhi3(i16 %a, i16 %b) #0 {
    ; CHECK-LABEL: call_mulhi3>
    ; CHECK: jsr __mulhi3
    %result = mul i16 %a, %b
    ret i16 %result
}

define i16 @call_divhi3(i16 %a, i16 %b) #0 {
    ; CHECK-LABEL: call_divhi3>
    ; CHECK: jsr __divhi3
    %result = sdiv i16 %a, %b
    ret i16 %result
}

define i32 @call_mulsi3(i32 %a, i32 %b) #0 {
    ; CHECK-LABEL: call_mulsi3>
    ; CHECK: jsr __mulsi3
    %result = mul i32 %a, %b
    ret i32 %result
}

; CHECK-LABEL: memcpy: ext
; CHECK-LABEL: memmove: ext
; CHECK-LABEL: __mulhi3: ext
; CHECK-LABEL: __divhi3: ext
; CHECK-LABEL: __mulsi3: ext
