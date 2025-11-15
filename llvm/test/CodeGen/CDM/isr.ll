target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for isr functions

; Function Attrs: noinline nounwind optnone
define cc228 void @some_isr_calling_fun() #0 {
entry:
; CHECK-LABEL: some_isr_calling_fun>
; CHECK-NOT: rts
; CHECK: rti
  ret void
}

attributes #0 = { noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
