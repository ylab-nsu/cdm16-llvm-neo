target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for different sections

; CHECK-LABEL: ### SECTION: .text
; CHECK: main>
; Function Attrs: noinline nounwind optnone
define i16 @main() #0 {
entry:
  ret i16 0
}

; CHECK-LABEL: ### SECTION: .rodata
; CHECK: b>
; CHECK-NEXT: dc 10
@b = constant i16 10, align 2

; CHECK-LABEL: ### SECTION: .data
; CHECK: d>
; CHECK-NEXT: dc 20
@d = global i16 20, align 2

; CHECK-LABEL: ### SECTION: .bss
; CHECK: c>
; CHECK-NEXT: dc 0
@c = global i16 0, align 2

attributes #0 = { noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

